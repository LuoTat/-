#include "precomp.hxx"

namespace hl
{

/****************************************************************************************\
*                                   logical operations                                   *
\****************************************************************************************/

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