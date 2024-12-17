#include "precomp.hxx"

/****************************************************************************************\
*                                   logical operations                                   *
\****************************************************************************************/

namespace hl
{

enum
{
    OCL_OP_ADD         = 0,
    OCL_OP_SUB         = 1,
    OCL_OP_RSUB        = 2,
    OCL_OP_ABSDIFF     = 3,
    OCL_OP_MUL         = 4,
    OCL_OP_MUL_SCALE   = 5,
    OCL_OP_DIV_SCALE   = 6,
    OCL_OP_RECIP_SCALE = 7,
    OCL_OP_ADDW        = 8,
    OCL_OP_AND         = 9,
    OCL_OP_OR          = 10,
    OCL_OP_XOR         = 11,
    OCL_OP_NOT         = 12,
    OCL_OP_MIN         = 13,
    OCL_OP_MAX         = 14,
    OCL_OP_RDIV_SCALE  = 15
};

static void binary_op(const Mat& _src1, const Mat& _src2, Mat& _dst, const Mat& _mask, const BinaryFuncC* tab, bool bitwise, int oclop)
{
    const Mat * psrc1 = &_src1, *psrc2 = &_src2;
    int         type1 = psrc1->type(), depth1 = HL_MAT_DEPTH(type1), cn = HL_MAT_CN(type1);
    int         type2 = psrc2->type(), depth2 = HL_MAT_DEPTH(type2), cn2 = HL_MAT_CN(type2);
    int         dims1 = psrc1->dims, dims2 = psrc2->dims;
    Size        sz1      = dims1 <= 2 ? psrc1->size() : Size();
    Size        sz2      = dims2 <= 2 ? psrc2->size() : Size();
    bool        haveMask = !_mask.empty(), haveScalar = false;
    BinaryFuncC func;

    if (dims1 <= 2 && dims2 <= 2 && sz1 == sz2 && type1 == type2 && !haveMask)
    {
        _dst.createSameSize(*psrc1, type1);

        if (bitwise)
        {
            func = *tab;
            cn   = (int)HL_ELEM_SIZE(type1);
        }
        else
        {
            func = tab[depth1];
        }
        HL_Assert(func);

        Mat    src1 = *psrc1, src2 = *psrc2, dst = _dst;
        Size   sz  = getContinuousSize2D(src1, src2, dst);
        size_t len = sz.width * (size_t)cn;
        if (len < INT_MAX)    // FIXIT similar code below doesn't have that check
        {
            sz.width = (int)len;
            func(src1.ptr(), src1.step, src2.ptr(), src2.step, dst.ptr(), dst.step, sz.width, sz.height, 0);
            return;
        }
    }

    if (oclop == OCL_OP_NOT)
        haveScalar = true;
    else if (!psrc1->sameSize(*psrc2) || type1 != type2)
    {
        if (checkScalar(*psrc1, type2))
        {
            // src1 is a scalar; swap it with src2
            swap(psrc1, psrc2);
            swap(type1, type2);
            swap(depth1, depth2);
            swap(cn, cn2);
            swap(sz1, sz2);
        }
        else if (!checkScalar(*psrc2, type1))
            HL_Error(hl::Error::StsUnmatchedSizes,
                     "The operation is neither 'array op array' (where arrays have the same size and type), "
                     "nor 'array op scalar', nor 'scalar op array'");
        haveScalar = true;
    }
    else
    {
        HL_Assert(psrc1->sameSize(*psrc2) && type1 == type2);
    }

    size_t     esz        = HL_ELEM_SIZE(type1);
    size_t     blocksize0 = (BLOCK_SIZE + esz - 1) / esz;
    BinaryFunc copymask   = 0;
    bool       reallocate = false;

    if (haveMask)
    {
        int mtype = _mask.type();
        HL_Assert((mtype == HL_8U || mtype == HL_8S) && _mask.sameSize(*psrc1));
        copymask   = getCopyMaskFunc(esz);
        reallocate = !_dst.sameSize(*psrc1) || _dst.type() != type1;
    }

    AutoBuffer<uchar> _buf;
    uchar *           scbuf = 0, *maskbuf = 0;

    _dst.createSameSize(*psrc1, type1);
    // if this is mask operation and dst has been reallocated,
    // we have to clear the destination
    if (haveMask && reallocate)
        _dst.setTo(0.);

    Mat src1 = *psrc1, src2 = *psrc2;
    Mat dst = _dst, mask = _mask;

    if (bitwise)
    {
        func = *tab;
        cn   = (int)esz;
    }
    else
        func = tab[depth1];
    HL_Assert(func);

    if (!haveScalar)
    {
        const Mat* arrays[] = {&src1, &src2, &dst, &mask, 0};
        uchar*     ptrs[4]  = {};

        NAryMatIterator it(arrays, ptrs);
        size_t          total = it.size, blocksize = total;

        if (blocksize * cn > INT_MAX)
            blocksize = INT_MAX / cn;

        if (haveMask)
        {
            blocksize = std::min(blocksize, blocksize0);
            _buf.allocate(blocksize * esz);
            maskbuf = _buf.data();
        }

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (size_t j = 0; j < total; j += blocksize)
            {
                int bsz = (int)MIN(total - j, blocksize);

                func(ptrs[0], 0, ptrs[1], 0, haveMask ? maskbuf : ptrs[2], 0, bsz * cn, 1, 0);
                if (haveMask)
                {
                    copymask(maskbuf, 0, ptrs[3], 0, ptrs[2], 0, Size(bsz, 1), &esz);
                    ptrs[3] += bsz;
                }

                bsz     *= (int)esz;
                ptrs[0] += bsz;
                ptrs[1] += bsz;
                ptrs[2] += bsz;
            }
        }
    }
    else
    {
        const Mat* arrays[] = {&src1, &dst, &mask, 0};
        uchar*     ptrs[3]  = {};

        NAryMatIterator it(arrays, ptrs);
        size_t          total = it.size, blocksize = std::min(total, blocksize0);

        _buf.allocate(blocksize * (haveMask ? 2 : 1) * esz + 32);
        scbuf   = _buf.data();
        maskbuf = alignPtr(scbuf + blocksize * esz, 16);

        convertAndUnrollScalar(src2, src1.type(), scbuf, blocksize);

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (size_t j = 0; j < total; j += blocksize)
            {
                int bsz = (int)MIN(total - j, blocksize);

                func(ptrs[0], 0, scbuf, 0, haveMask ? maskbuf : ptrs[1], 0, bsz * cn, 1, 0);
                if (haveMask)
                {
                    copymask(maskbuf, 0, ptrs[2], 0, ptrs[1], 0, Size(bsz, 1), &esz);
                    ptrs[2] += bsz;
                }

                bsz     *= (int)esz;
                ptrs[0] += bsz;
                ptrs[1] += bsz;
            }
        }
    }
}

static BinaryFuncC* getMaxTab()
{
    static BinaryFuncC maxTab[HL_DEPTH_MAX] = {(BinaryFuncC)hl::hal::max8u, (BinaryFuncC)hl::hal::max8s, (BinaryFuncC)hl::hal::max16u, (BinaryFuncC)hl::hal::max16s, (BinaryFuncC)hl::hal::max32u, (BinaryFuncC)hl::hal::max32s, (BinaryFuncC)hl::hal::max32f, (BinaryFuncC)hl::hal::max64f};

    return maxTab;
}

static BinaryFuncC* getMinTab()
{
    static BinaryFuncC minTab[HL_DEPTH_MAX] = {(BinaryFuncC)hl::hal::min8u, (BinaryFuncC)hl::hal::min8s, (BinaryFuncC)hl::hal::min16u, (BinaryFuncC)hl::hal::min16s, (BinaryFuncC)hl::hal::min32u, (BinaryFuncC)hl::hal::min32s, (BinaryFuncC)hl::hal::min32f, (BinaryFuncC)hl::hal::min64f};

    return minTab;
}

}    // namespace hl

void hl::bitwise_and(const Mat& a, const Mat& b, Mat& c, const Mat& mask)
{
    BinaryFuncC f = (BinaryFuncC)hl::hal::and8u;
    binary_op(a, b, c, mask, &f, true, OCL_OP_AND);
}

void hl::bitwise_or(const Mat& a, const Mat& b, Mat& c, const Mat& mask)
{
    BinaryFuncC f = (BinaryFuncC)hl::hal::or8u;
    binary_op(a, b, c, mask, &f, true, OCL_OP_OR);
}

void hl::bitwise_xor(const Mat& a, const Mat& b, Mat& c, const Mat& mask)
{
    BinaryFuncC f = (BinaryFuncC)hl::hal::xor8u;
    binary_op(a, b, c, mask, &f, true, OCL_OP_XOR);
}

void hl::bitwise_not(const Mat& a, Mat& c, const Mat& mask)
{
    BinaryFuncC f = (BinaryFuncC)hl::hal::not8u;
    binary_op(a, a, c, mask, &f, true, OCL_OP_NOT);
}

void hl::max(const Mat& src1, const Mat& src2, Mat& dst)
{
    binary_op(src1, src2, dst, Mat(), getMaxTab(), false, OCL_OP_MAX);
}

void hl::min(const Mat& src1, const Mat& src2, Mat& dst)
{
    binary_op(src1, src2, dst, Mat(), getMinTab(), false, OCL_OP_MIN);
}

/****************************************************************************************\
*                                      add/subtract                                      *
\****************************************************************************************/

namespace hl
{

typedef int (*ScalarFunc)(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool scalarIsFirst, int nChannels);

typedef int (*ExtendedTypeFunc)(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);

static int actualScalarDepth(const double* data, int len)
{
    int i = 0, minval = INT_MAX, maxval = INT_MIN;
    for (; i < len; ++i)
    {
        int ival = hlRound(data[i]);
        if (ival != data[i])
            break;
        minval = MIN(minval, ival);
        maxval = MAX(maxval, ival);
    }
    return i < len ? HL_64F : minval >= 0 && maxval <= (int)UCHAR_MAX          ? HL_8U
                        : minval >= (int)SCHAR_MIN && maxval <= (int)SCHAR_MAX ? HL_8S
                        : minval >= 0 && maxval <= (int)USHRT_MAX              ? HL_16U
                        : minval >= (int)SHRT_MIN && maxval <= (int)SHRT_MAX   ? HL_16S
                                                                               : HL_32S;
}

static void arithm_op(const Mat& _src1, const Mat& _src2, Mat& _dst, const Mat& _mask, int dtype, BinaryFuncC* tab, bool muldiv = false, void* usrdata = 0, int oclop = -1, ExtendedTypeFunc extendedFunc = nullptr, ScalarFunc scalarFunc = nullptr)
{
    const Mat *psrc1 = &_src1, *psrc2 = &_src2;
    bool       haveMask   = !_mask.empty();
    bool       reallocate = false;
    int        type1 = psrc1->type(), depth1 = HL_MAT_DEPTH(type1), cn = HL_MAT_CN(type1);
    int        type2 = psrc2->type(), depth2 = HL_MAT_DEPTH(type2), cn2 = HL_MAT_CN(type2);
    int        wtype, dims1 = psrc1->dims, dims2 = psrc2->dims;
    Size       sz1        = dims1 <= 2 ? psrc1->size() : Size();
    Size       sz2        = dims2 <= 2 ? psrc2->size() : Size();
    bool       src1Scalar = checkScalar(*psrc1, type2);
    bool       src2Scalar = checkScalar(*psrc2, type1);

    if (cn == 1 && sz1 == sz2 && dims1 <= 2 && dims2 <= 2 && type1 == type2 && !haveMask && _dst.type() == type1 && src1Scalar == src2Scalar)
    {
        _dst.createSameSize(*psrc1, type1);

        Mat  src1 = *psrc1, src2 = *psrc2, dst = _dst;
        Size sz = getContinuousSize2D(src1, src2, dst, src1.channels());
        if (!extendedFunc || extendedFunc(src1.ptr(), src1.step, src2.ptr(), src2.step, dst.ptr(), dst.step, sz.width, sz.height, usrdata) != 0)
        {
            BinaryFuncC func = tab[depth1];
            HL_Assert(func);
            func(src1.ptr(), src1.step, src2.ptr(), src2.step, dst.ptr(), dst.step, sz.width, sz.height, usrdata);
        }
        return;
    }

    bool haveScalar = false, swapped12 = false;

    if (dims1 != dims2 || sz1 != sz2 || cn != cn2)
    {
        if ((type1 == HL_64F && (sz1.height == 1 || sz1.height == 4)) && src1Scalar)
        {
            // src1 is a scalar; swap it with src2
            swap(psrc1, psrc2);
            swap(sz1, sz2);
            swap(type1, type2);
            swap(depth1, depth2);
            swap(cn, cn2);
            swap(dims1, dims2);
            swapped12 = true;
            if (oclop == OCL_OP_SUB)
                oclop = OCL_OP_RSUB;
            if (oclop == OCL_OP_DIV_SCALE)
                oclop = OCL_OP_RDIV_SCALE;
        }
        else if (!src2Scalar)
            HL_Error(hl::Error::StsUnmatchedSizes,
                     "The operation is neither 'array op array' "
                     "(where arrays have the same size and the same number of channels), "
                     "nor 'array op scalar', nor 'scalar op array'");
        haveScalar = true;
        HL_Assert(type2 == HL_64F && (sz2.height == 1 || sz2.height == 4));

        if (!muldiv)
        {
            Mat sc = *psrc2;
            depth2 = actualScalarDepth(sc.ptr<double>(), sz2 == Size(1, 1) ? cn2 : cn);
            if (depth2 == HL_64F && (depth1 < HL_32S || depth1 == HL_32F))
                depth2 = HL_32F;
        }
        else
            depth2 = HL_64F;
    }

    if (dtype < 0)
        dtype = _dst.type();

    dtype = HL_MAT_DEPTH(dtype);

    if (depth1 == depth2 && dtype == depth1)
        wtype = dtype;
    else if (!muldiv)
    {
        wtype = depth1 <= HL_8S && depth2 <= HL_8S ? HL_16S : depth1 <= HL_32S && depth2 <= HL_32S ? HL_32S
                                                                                                   : std::max(depth1, depth2);
        wtype = std::max(wtype, dtype);

        // when the result of addition should be converted to an integer type,
        // and just one of the input arrays is floating-point, it makes sense to convert that input to integer type before the operation,
        // instead of converting the other input to floating-point and then converting the operation result back to integers.
        if (dtype < HL_32F && (depth1 < HL_32F || depth2 < HL_32F))
            wtype = HL_32S;
    }
    else
    {
        wtype = std::max(depth1, std::max(depth2, HL_32F));
        wtype = std::max(wtype, dtype);
    }

    dtype = HL_MAKETYPE(dtype, cn);
    wtype = HL_MAKETYPE(wtype, cn);

    if (haveMask)
    {
        int mtype = _mask.type();
        HL_Assert((mtype == HL_8UC1 || mtype == HL_8SC1) && _mask.sameSize(*psrc1));
        reallocate = !_dst.sameSize(*psrc1) || _dst.type() != dtype;
    }

    _dst.createSameSize(*psrc1, dtype);
    if (reallocate)
        _dst.setTo(0.);

    BinaryFunc cvtsrc1 = type1 == wtype ? 0 : getConvertFunc(type1, wtype);
    BinaryFunc cvtsrc2 = type2 == type1 ? cvtsrc1 : type2 == wtype ? 0
                                                                   : getConvertFunc(type2, wtype);
    BinaryFunc cvtdst  = dtype == wtype ? 0 : getConvertFunc(wtype, dtype);

    size_t     esz1 = HL_ELEM_SIZE(type1), esz2 = HL_ELEM_SIZE(type2);
    size_t     dsz = HL_ELEM_SIZE(dtype), wsz = HL_ELEM_SIZE(wtype);
    size_t     blocksize0 = (size_t)(BLOCK_SIZE + wsz - 1) / wsz;
    BinaryFunc copymask   = getCopyMaskFunc(dsz);
    Mat        src1 = *psrc1, src2 = *psrc2, dst = _dst, mask = _mask;

    AutoBuffer<uchar> _buf;
    uchar *           buf, *maskbuf = 0, *buf1 = 0, *buf2 = 0, *wbuf = 0;
    size_t            bufesz = (cvtsrc1 ? wsz : 0) + (cvtsrc2 || haveScalar ? wsz : 0) + (cvtdst ? wsz : 0) + (haveMask ? dsz : 0);
    BinaryFuncC       func   = tab[HL_MAT_DEPTH(wtype)];
    HL_Assert(func);

    if (!haveScalar)
    {
        const Mat* arrays[] = {&src1, &src2, &dst, &mask, 0};
        uchar*     ptrs[4]  = {};

        NAryMatIterator it(arrays, ptrs);
        size_t          total = it.size, blocksize = total;

        if (haveMask || cvtsrc1 || cvtsrc2 || cvtdst)
            blocksize = std::min(blocksize, blocksize0);

        _buf.allocate(bufesz * blocksize + 64);
        buf = _buf.data();
        if (cvtsrc1)
        {
            buf1 = buf, buf = alignPtr(buf + blocksize * wsz, 16);
        }
        if (cvtsrc2)
        {
            buf2 = buf, buf = alignPtr(buf + blocksize * wsz, 16);
        }
        wbuf = maskbuf = buf;
        if (cvtdst)
        {
            buf = alignPtr(buf + blocksize * wsz, 16);
        }
        if (haveMask)
        {
            maskbuf = buf;
        }

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (size_t j = 0; j < total; j += blocksize)
            {
                int          bsz = (int)MIN(total - j, blocksize);
                Size         bszn(bsz * cn, 1);
                const uchar *sptr1 = ptrs[0], *sptr2 = ptrs[1];
                uchar*       dptr  = ptrs[2];
                // try to perform operation with conversion in one call
                // if fail, use converter functions
                uchar* opconverted = haveMask ? maskbuf : dptr;
                if (!extendedFunc || extendedFunc(sptr1, 1, sptr2, 1, opconverted, (!haveMask), bszn.width, bszn.height, usrdata) != 0)
                {
                    if (cvtsrc1)
                    {
                        cvtsrc1(sptr1, 1, 0, 1, buf1, 1, bszn, 0);
                        sptr1 = buf1;
                    }
                    if (ptrs[0] == ptrs[1])
                    {
                        sptr2 = sptr1;
                    }
                    else if (cvtsrc2)
                    {
                        cvtsrc2(sptr2, 1, 0, 1, buf2, 1, bszn, 0);
                        sptr2 = buf2;
                    }

                    uchar* fdst = (haveMask || cvtdst) ? wbuf : dptr;
                    func(sptr1, 1, sptr2, 1, fdst, (!haveMask && !cvtdst), bszn.width, bszn.height, usrdata);

                    if (cvtdst)
                    {
                        uchar* cdst = haveMask ? maskbuf : dptr;
                        cvtdst(wbuf, 1, 0, 1, cdst, 1, bszn, 0);
                    }
                    opconverted = cvtdst ? maskbuf : wbuf;
                }

                if (haveMask)
                {
                    copymask(opconverted, 1, ptrs[3], 1, dptr, 1, Size(bsz, 1), &dsz);
                    ptrs[3] += bsz;
                }

                ptrs[0] += bsz * esz1;
                ptrs[1] += bsz * esz2;
                ptrs[2] += bsz * dsz;
            }
        }
    }
    else
    {
        const Mat* arrays[] = {&src1, &dst, &mask, 0};
        uchar*     ptrs[3]  = {};

        NAryMatIterator it(arrays, ptrs);
        size_t          total = it.size, blocksize = std::min(total, blocksize0);

        _buf.allocate(bufesz * blocksize + 64);
        buf = _buf.data();
        if (cvtsrc1)
        {
            buf1 = buf, buf = alignPtr(buf + blocksize * wsz, 16);
        }
        buf2 = buf;
        buf  = alignPtr(buf + blocksize * wsz, 16);
        wbuf = maskbuf = buf;
        if (cvtdst)
        {
            buf = alignPtr(buf + blocksize * wsz, 16);
        }
        if (haveMask)
        {
            maskbuf = buf;
        }

        convertAndUnrollScalar(src2, wtype, buf2, blocksize);

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (size_t j = 0; j < total; j += blocksize)
            {
                int          bsz      = (int)MIN(total - j, blocksize);
                const uchar* sptr1    = ptrs[0];
                const uchar* sptr2    = buf2;
                uchar*       dptr     = ptrs[1];

                const uchar* extSptr1 = sptr1;
                const uchar* extSptr2 = sptr2;
                if (swapped12)
                    std::swap(extSptr1, extSptr2);

                // try to perform operation in 1 call, fallback to classic way if fail
                uchar* opconverted = haveMask ? maskbuf : dptr;
                if (!scalarFunc || src2.total() != 1 || scalarFunc(extSptr1, 1, opconverted, 1, bsz, 1, (void*)extSptr2, swapped12, cn) != 0)
                {
                    // try to perform operation with conversion in one call
                    // if fail, use converter functions

                    if (!extendedFunc || extendedFunc(extSptr1, 1, extSptr2, 1, opconverted, 1, bsz * cn, 1, usrdata) != 0)
                    {
                        if (cvtsrc1)
                        {
                            cvtsrc1(sptr1, 1, 0, 1, buf1, 1, Size(bsz * cn, 1), 0);
                            sptr1 = buf1;
                        }

                        if (swapped12)
                            std::swap(sptr1, sptr2);

                        uchar* fdst = (haveMask || cvtdst) ? wbuf : dptr;
                        func(sptr1, 1, sptr2, 1, fdst, 1, bsz * cn, 1, usrdata);

                        if (cvtdst)
                        {
                            uchar* cdst = haveMask ? maskbuf : dptr;
                            cvtdst(wbuf, 1, 0, 1, cdst, 1, Size(bsz * cn, 1), 0);
                        }
                        opconverted = cvtdst ? maskbuf : wbuf;
                    }
                }

                if (haveMask)
                {
                    copymask(opconverted, 1, ptrs[2], 1, dptr, 1, Size(bsz, 1), &dsz);
                    ptrs[2] += bsz;
                }

                ptrs[0] += bsz * esz1;
                ptrs[1] += bsz * dsz;
            }
        }
    }
}

static BinaryFuncC* getAddTab()
{
    static BinaryFuncC addTab[HL_DEPTH_MAX] = {(BinaryFuncC)hl::hal::add8u, (BinaryFuncC)hl::hal::add8s, (BinaryFuncC)hl::hal::add16u, (BinaryFuncC)hl::hal::add16s, (BinaryFuncC)hl::hal::add32u, (BinaryFuncC)hl::hal::add32s, (BinaryFuncC)hl::hal::add32f, (BinaryFuncC)hl::hal::add64f};

    return addTab;
}

static int addScalar32f32fWrapper(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool /*scalarIsFirst*/, int nChannels)
{
    HL_UNUSED(src);
    HL_UNUSED(step_src);
    HL_UNUSED(dst);
    HL_UNUSED(step_dst);
    HL_UNUSED(width);
    HL_UNUSED(height);
    HL_UNUSED(scalar);
    HL_UNUSED(nChannels);
    return 1;
}

static int addScalar16s16sWrapper(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool /*scalarIsFirst*/, int nChannels)
{
    HL_UNUSED(src);
    HL_UNUSED(step_src);
    HL_UNUSED(dst);
    HL_UNUSED(step_dst);
    HL_UNUSED(width);
    HL_UNUSED(height);
    HL_UNUSED(scalar);
    HL_UNUSED(nChannels);
    return 1;
}

static ScalarFunc getAddScalarFunc(int srcType, int dstType)
{
    if (srcType == HL_32F && dstType == HL_32F)
    {
        return addScalar32f32fWrapper;
    }
    else if (srcType == HL_16S && dstType == HL_16S)
    {
        return addScalar16s16sWrapper;
    }
    else
    {
        return nullptr;
    }
}

static int sub8u32fWrapper(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*)
{
    HL_UNUSED(src1);
    HL_UNUSED(step1);
    HL_UNUSED(src2);
    HL_UNUSED(step2);
    HL_UNUSED(dst);
    HL_UNUSED(step);
    HL_UNUSED(width);
    HL_UNUSED(height);
    return 1;
}

static int sub8s32fWrapper(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*)
{
    HL_UNUSED(src1);
    HL_UNUSED(step1);
    HL_UNUSED(src2);
    HL_UNUSED(step2);
    HL_UNUSED(dst);
    HL_UNUSED(step);
    HL_UNUSED(width);
    HL_UNUSED(height);
    return 1;
}

static BinaryFuncC* getSubTab()
{
    static BinaryFuncC subTab[HL_DEPTH_MAX] = {(BinaryFuncC)hl::hal::sub8u, (BinaryFuncC)hl::hal::sub8s, (BinaryFuncC)hl::hal::sub16u, (BinaryFuncC)hl::hal::sub16s, (BinaryFuncC)hl::hal::sub32u, (BinaryFuncC)hl::hal::sub32s, (BinaryFuncC)hl::hal::sub32f, (BinaryFuncC)hl::hal::sub64f};

    return subTab;
}

static ExtendedTypeFunc getSubExtFunc(int src1Type, int src2Type, int dstType)
{
    if (src1Type == HL_8U && src2Type == HL_8U && dstType == HL_32F)
    {
        return sub8u32fWrapper;
    }
    else if (src1Type == HL_8S && src2Type == HL_8S && dstType == HL_32F)
    {
        return sub8s32fWrapper;
    }
    else
    {
        return nullptr;
    }
}

static BinaryFuncC* getAbsDiffTab()
{
    static BinaryFuncC absDiffTab[HL_DEPTH_MAX] = {(BinaryFuncC)hl::hal::absdiff8u, (BinaryFuncC)hl::hal::absdiff8s, (BinaryFuncC)hl::hal::absdiff16u, (BinaryFuncC)hl::hal::absdiff16s, (BinaryFuncC)hl::hal::absdiff32u, (BinaryFuncC)hl::hal::absdiff32s, (BinaryFuncC)hl::hal::absdiff32f, (BinaryFuncC)hl::hal::absdiff64f};

    return absDiffTab;
}

static int absDiffScalar32f32fWrapper(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool /*scalarIsFirst*/, int nChannels)
{
    HL_UNUSED(src);
    HL_UNUSED(step_src);
    HL_UNUSED(dst);
    HL_UNUSED(step_dst);
    HL_UNUSED(width);
    HL_UNUSED(height);
    HL_UNUSED(scalar);
    HL_UNUSED(nChannels);
    return 1;
}

static int absDiffScalar32s32uWrapper(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool /*scalarIsFirst*/, int nChannels)
{
    HL_UNUSED(src);
    HL_UNUSED(step_src);
    HL_UNUSED(dst);
    HL_UNUSED(step_dst);
    HL_UNUSED(width);
    HL_UNUSED(height);
    HL_UNUSED(scalar);
    HL_UNUSED(nChannels);
    return 1;
}

static int absDiffScalar8u8uWrapper(const uchar* src, size_t step_src, uchar* dst, size_t step_dst, int width, int height, void* scalar, bool /*scalarIsFirst*/, int nChannels)
{
    HL_UNUSED(src);
    HL_UNUSED(step_src);
    HL_UNUSED(dst);
    HL_UNUSED(step_dst);
    HL_UNUSED(width);
    HL_UNUSED(height);
    HL_UNUSED(scalar);
    HL_UNUSED(nChannels);
    return 1;
}

static ScalarFunc getAbsDiffScalarFunc(int srcType, int dstType)
{
    if (srcType == HL_32F && dstType == HL_32F)
    {
        return absDiffScalar32f32fWrapper;
    }
    // resulting type is 32U in fact
    else if (srcType == HL_32S && dstType == HL_32S)
    {
        return absDiffScalar32s32uWrapper;
    }
    else if (srcType == HL_8U && dstType == HL_8U)
    {
        return absDiffScalar8u8uWrapper;
    }
    else
    {
        return nullptr;
    }
}

}    // namespace hl

void hl::add(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask, int dtype)
{
    HL_Assert(src1.empty() == src2.empty());
    if (src1.empty() && src2.empty())
    {
        dst.release();
        return;
    }

    int sdepth = src1.depth();
    if (checkScalar(src1, src1.type()))
    {
        sdepth = src2.depth();
    }
    if (checkScalar(src2, src2.type()))
    {
        sdepth = src1.depth();
    }

    ScalarFunc scalarFunc = getAddScalarFunc(sdepth, dtype < 0 ? dst.depth() : dtype);
    arithm_op(src1, src2, dst, mask, dtype, getAddTab(), false, 0, OCL_OP_ADD, nullptr,
              /* scalarFunc */ scalarFunc);
}

void hl::subtract(const Mat& _src1, const Mat& _src2, Mat& _dst, const Mat& mask, int dtype)
{
    HL_Assert(_src1.empty() == _src2.empty());
    if (_src1.empty() && _src2.empty())
    {
        _dst.release();
        return;
    }

    ExtendedTypeFunc subExtFunc = getSubExtFunc(_src1.depth(), _src2.depth(), dtype < 0 ? _dst.depth() : dtype);
    arithm_op(_src1, _src2, _dst, mask, dtype, getSubTab(), false, 0, OCL_OP_SUB,
              /* extendedFunc */ subExtFunc);
}

void hl::absdiff(const Mat& src1, const Mat& src2, Mat& dst)
{
    HL_Assert(src1.empty() == src2.empty());
    if (src1.empty() && src2.empty())
    {
        dst.release();
        return;
    }

    int sdepth = src1.depth();
    if (checkScalar(src1, src1.type()))
    {
        sdepth = src2.depth();
    }
    if (checkScalar(src2, src2.type()))
    {
        sdepth = src1.depth();
    }
    ScalarFunc scalarFunc = getAbsDiffScalarFunc(sdepth, dst.depth());
    arithm_op(src1, src2, dst, Mat(), -1, getAbsDiffTab(), false, 0, OCL_OP_ABSDIFF,
              /* extendedFunc */ nullptr,
              scalarFunc);
}

void hl::copyTo(const Mat& _src, Mat& _dst, const Mat& _mask)
{
    _src.copyTo(_dst, _mask);
}