#include "precomp.hxx"

namespace hl
{

template <typename T>
inline static void scalarToRawData_(const Scalar& s, T* const buf, const int cn, const int unroll_to)
{
    int i = 0;
    for (; i < cn; i++)
        buf[i] = saturate_cast<T>(s.val[i]);
    for (; i < unroll_to; i++)
        buf[i] = buf[i - cn];
}

void scalarToRawData(const Scalar& s, void* _buf, int type, int unroll_to)
{
    const int depth = HL_MAT_DEPTH(type), cn = HL_MAT_CN(type);
    HL_Assert(cn <= 4);
    switch (depth)
    {
        case HL_8U :
            scalarToRawData_<uchar>(s, (uchar*)_buf, cn, unroll_to);
            break;
        case HL_8S :
            scalarToRawData_<schar>(s, (schar*)_buf, cn, unroll_to);
            break;
        case HL_16U :
            scalarToRawData_<ushort>(s, (ushort*)_buf, cn, unroll_to);
            break;
        case HL_16S :
            scalarToRawData_<short>(s, (short*)_buf, cn, unroll_to);
            break;
        case HL_32U :
            scalarToRawData_<uint>(s, (uint*)_buf, cn, unroll_to);
            break;
        case HL_32S :
            scalarToRawData_<int>(s, (int*)_buf, cn, unroll_to);
            break;
        case HL_32F :
            scalarToRawData_<float>(s, (float*)_buf, cn, unroll_to);
            break;
        case HL_64F :
            scalarToRawData_<double>(s, (double*)_buf, cn, unroll_to);
            break;
        default :
            HL_Error(HL_StsUnsupportedFormat, "");
    }
}

void convertAndUnrollScalar(const Mat& sc, int buftype, uchar* scbuf, size_t blocksize)
{
    int        scn = (int)sc.total(), cn = HL_MAT_CN(buftype);
    size_t     esz   = HL_ELEM_SIZE(buftype);
    BinaryFunc cvtFn = getConvertFunc(sc.depth(), buftype);
    HL_Assert(cvtFn);
    cvtFn(sc.ptr(), 1, 0, 1, scbuf, 1, Size(std::min(cn, scn), 1), 0);
    // unroll the scalar
    if (scn < cn)
    {
        HL_Assert(scn == 1);
        size_t esz1 = HL_ELEM_SIZE1(buftype);
        for (size_t i = esz1; i < esz; i++)
            scbuf[i] = scbuf[i - esz1];
    }
    for (size_t i = esz; i < blocksize * esz; i++)
        scbuf[i] = scbuf[i - esz];
}

template <typename T>
static void copyMask_(const uchar* _src, size_t sstep, const uchar* mask, size_t mstep, uchar* _dst, size_t dstep, Size size)
{
    for (; size.height--; mask += mstep, _src += sstep, _dst += dstep)
    {
        const T* src = (const T*)_src;
        T*       dst = (T*)_dst;
        for (int x = 0; x < size.width; x++)
            if (mask[x])
                dst[x] = src[x];
    }
}

static void copyMaskGeneric(const uchar* _src, size_t sstep, const uchar* mask, size_t mstep, uchar* _dst, size_t dstep, Size size, void* _esz)
{
    size_t k, esz = *(size_t*)_esz;
    for (; size.height--; mask += mstep, _src += sstep, _dst += dstep)
    {
        const uchar* src = _src;
        uchar*       dst = _dst;
        int          x   = 0;
        for (; x < size.width; x++, src += esz, dst += esz)
        {
            if (!mask[x])
                continue;
            for (k = 0; k < esz; k++)
                dst[k] = src[k];
        }
    }
}

#define DEF_COPY_MASK(suffix, type)                                                                                                           \
    static void copyMask##suffix(const uchar* src, size_t sstep, const uchar* mask, size_t mstep, uchar* dst, size_t dstep, Size size, void*) \
    {                                                                                                                                         \
        copyMask_<type>(src, sstep, mask, mstep, dst, dstep, size);                                                                           \
    }

DEF_COPY_MASK(8u, uchar)
DEF_COPY_MASK(16u, ushort)
DEF_COPY_MASK(8uC3, Vec3b)
DEF_COPY_MASK(32s, int)
DEF_COPY_MASK(16uC3, Vec3s)
DEF_COPY_MASK(32sC2, Vec2i)
DEF_COPY_MASK(32sC3, Vec3i)
DEF_COPY_MASK(32sC4, Vec4i)
DEF_COPY_MASK(32sC6, Vec6i)
DEF_COPY_MASK(32sC8, Vec8i)

BinaryFunc copyMaskTab[] = {
    0,
    copyMask8u,
    copyMask16u,
    copyMask8uC3,
    copyMask32s,
    0,
    copyMask16uC3,
    0,
    copyMask32sC2,
    0,
    0,
    0,
    copyMask32sC3,
    0,
    0,
    0,
    copyMask32sC4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    copyMask32sC6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    copyMask32sC8};

BinaryFunc getCopyMaskFunc(size_t esz)
{
    return esz <= 32 && copyMaskTab[esz] ? copyMaskTab[esz] : copyMaskGeneric;
}

/* dst = src */
void Mat::copyTo(Mat& _dst) const
{
    int dtype = _dst.type();
    if (dtype != type())
    {
        HL_Assert(channels() == HL_MAT_CN(dtype));
        convertTo(_dst, dtype);
        return;
    }

    if (empty())
    {
        _dst.release();
        return;
    }

    if (dims <= 2)
    {
        _dst.create(rows, cols, type());
        Mat dst = _dst;
        if (data == dst.data)
            return;

        if (rows > 0 && cols > 0)
        {
            Mat  src = *this;
            Size sz  = getContinuousSize2D(src, dst, (int)elemSize());
            HL_CheckGE(sz.width, 0, "");

            const uchar* sptr = src.data;
            uchar*       dptr = dst.data;

            for (; sz.height--; sptr += src.step, dptr += dst.step)
                memcpy(dptr, sptr, sz.width);
        }
        return;
    }

    _dst.create(dims, size, type());
    Mat dst = _dst;
    if (data == dst.data)
        return;

    if (total() != 0)
    {
        const Mat*      arrays[] = {this, &dst};
        uchar*          ptrs[2]  = {};
        NAryMatIterator it(arrays, ptrs, 2);
        size_t          sz = it.size * elemSize();

        for (size_t i = 0; i < it.nplanes; i++, ++it)
            memcpy(ptrs[1], ptrs[0], sz);
    }
}

void Mat::copyTo(Mat& _dst, const Mat& _mask) const
{
    Mat mask = _mask;
    if (!mask.data)
    {
        copyTo(_dst);
        return;
    }

    int cn = channels(), mcn = mask.channels();
    HL_Assert(mask.depth() == HL_8U && (mcn == 1 || mcn == cn));
    bool colorMask = mcn > 1;
    if (dims <= 2)
    {
        HL_Assert(size() == mask.size());
    }

    Mat dst;
    {
        Mat dst0 = _dst;
        _dst.create(dims, size, type());    // TODO Prohibit 'dst' re-creation, user should pass it explicitly with correct size/type or empty
        dst = _dst;

        if (dst.data != dst0.data)          // re-allocation happened
        {
            dst = Scalar(0);                // do not leave dst uninitialized
        }
    }

    size_t     esz      = colorMask ? elemSize1() : elemSize();
    BinaryFunc copymask = getCopyMaskFunc(esz);

    if (dims <= 2)
    {
        Mat  src = *this;
        Size sz  = getContinuousSize2D(src, dst, mask, mcn);
        copymask(src.data, src.step, mask.data, mask.step, dst.data, dst.step, sz, &esz);
        return;
    }

    const Mat*      arrays[] = {this, &dst, &mask, 0};
    uchar*          ptrs[3]  = {};
    NAryMatIterator it(arrays, ptrs);
    Size            sz((int)(it.size * mcn), 1);

    for (size_t i = 0; i < it.nplanes; i++, ++it)
        copymask(ptrs[0], 0, ptrs[2], 0, ptrs[1], 0, sz, &esz);
}

static bool can_apply_memset(const Mat& mat, const Scalar& s, int& fill_value)
{
    // check if depth is 1 byte.
    switch (mat.depth())
    {
        case HL_8U : fill_value = saturate_cast<uchar>(s.val[0]); break;
        case HL_8S : fill_value = saturate_cast<schar>(s.val[0]); break;
        default    : return false;
    }

    // check if all element is same.
    const int64* is = (const int64*)&s.val[0];
    switch (mat.channels())
    {
        case 1  : return true;
        case 2  : return (is[0] == is[1]);
        case 3  : return (is[0] == is[1] && is[1] == is[2]);
        case 4  : return (is[0] == is[1] && is[1] == is[2] && is[2] == is[3]);
        default : return false;
    }
}

Mat& Mat::operator=(const Scalar& s)
{
    if (this->empty())
        return *this;

    const Mat*      arrays[] = {this};
    uchar*          dptr;
    NAryMatIterator it(arrays, &dptr, 1);
    size_t          elsize = it.size * elemSize();
    const int64*    is     = (const int64*)&s.val[0];

    if (is[0] == 0 && is[1] == 0 && is[2] == 0 && is[3] == 0)
    {
        for (size_t i = 0; i < it.nplanes; i++, ++it)
            memset(dptr, 0, elsize);
    }
    else
    {
        int fill_value = 0;
        if (can_apply_memset(*this, s, fill_value))
        {
            for (size_t i = 0; i < it.nplanes; i++, ++it)
                memset(dptr, fill_value, elsize);
            return *this;
        }

        if (it.nplanes > 0)
        {
            double scalar[12];
            scalarToRawData(s, scalar, type(), 12);
            size_t blockSize = 12 * elemSize1();

            for (size_t j = 0; j < elsize; j += blockSize)
            {
                size_t sz = MIN(blockSize, elsize - j);
                HL_Assert(sz <= sizeof(scalar));
                memcpy(dptr + j, scalar, sz);
            }
        }

        for (size_t i = 1; i < it.nplanes; i++)
        {
            ++it;
            memcpy(dptr, data, elsize);
        }
    }
    return *this;
}

Mat& Mat::setTo(const Mat& _value, const Mat& _mask)
{
    if (empty())
        return *this;

    Mat value = _value, mask = _mask;

    HL_Assert(checkScalar(value, type()));
    int cn = channels(), mcn = mask.channels();
    HL_Assert(mask.empty() || (mask.depth() == HL_8U && (mcn == 1 || mcn == cn) && size == mask.size));

    size_t     esz           = mcn > 1 ? elemSize1() : elemSize();
    BinaryFunc copymask      = getCopyMaskFunc(esz);

    const Mat*      arrays[] = {this, !mask.empty() ? &mask : 0, 0};
    uchar*          ptrs[2]  = {0, 0};
    NAryMatIterator it(arrays, ptrs);
    int             totalsz     = (int)it.size * mcn;
    int             blockSize0  = std::min(totalsz, (int)((BLOCK_SIZE + esz - 1) / esz));
    blockSize0                 -= blockSize0 % mcn;    // must be divisible without remainder for unrolling and advancing
    AutoBuffer<uchar> _scbuf(blockSize0 * esz + 32);
    uchar*            scbuf = alignPtr((uchar*)_scbuf.data(), (int)sizeof(double));
    convertAndUnrollScalar(value, type(), scbuf, blockSize0 / mcn);

    for (size_t i = 0; i < it.nplanes; i++, ++it)
    {
        for (int j = 0; j < totalsz; j += blockSize0)
        {
            Size   sz(std::min(blockSize0, totalsz - j), 1);
            size_t blockSize = sz.width * esz;
            if (ptrs[1])
            {
                copymask(scbuf, 0, ptrs[1], 0, ptrs[0], 0, sz, &esz);
                ptrs[1] += sz.width;
            }
            else
                memcpy(ptrs[0], scbuf, blockSize);
            ptrs[0] += blockSize;
        }
    }
    return *this;
}
}    // namespace hl

int hl::borderInterpolate(int p, int len, int borderType)
{
    HL_DbgAssert(len > 0);

    if ((unsigned)p < (unsigned)len)
        ;
    else if (borderType == BORDER_REPLICATE)
        p = p < 0 ? 0 : len - 1;
    else if (borderType == BORDER_REFLECT || borderType == BORDER_REFLECT_101)
    {
        int delta = borderType == BORDER_REFLECT_101;
        if (len == 1)
            return 0;
        do
        {
            if (p < 0)
                p = -p - 1 + delta;
            else
                p = len - 1 - (p - len) - delta;
        }
        while ((unsigned)p >= (unsigned)len);
    }
    else if (borderType == BORDER_WRAP)
    {
        HL_Assert(len > 0);
        if (p < 0)
            p -= ((p - len + 1) / len) * len;
        if (p >= len)
            p %= len;
    }
    else if (borderType == BORDER_CONSTANT)
        p = -1;
    else
        HL_Error(HL_StsBadArg, "Unknown/unsupported border type");
    return p;
}

namespace
{

void copyMakeBorder_8u(const uchar* src, size_t srcstep, hl::Size srcroi, uchar* dst, size_t dststep, hl::Size dstroi, int top, int left, int cn, int borderType)
{
    const int isz = (int)sizeof(int);
    int       i, j, k, elemSize = 1;
    bool      intMode = false;

    if ((cn | srcstep | dststep | (size_t)src | (size_t)dst) % isz == 0)
    {
        cn       /= isz;
        elemSize  = isz;
        intMode   = true;
    }

    hl::AutoBuffer<int> _tab((dstroi.width - srcroi.width) * cn);
    int*                tab    = _tab.data();
    int                 right  = dstroi.width - srcroi.width - left;
    int                 bottom = dstroi.height - srcroi.height - top;

    for (i = 0; i < left; i++)
    {
        j = hl::borderInterpolate(i - left, srcroi.width, borderType) * cn;
        for (k = 0; k < cn; k++)
            tab[i * cn + k] = j + k;
    }

    for (i = 0; i < right; i++)
    {
        j = hl::borderInterpolate(srcroi.width + i, srcroi.width, borderType) * cn;
        for (k = 0; k < cn; k++)
            tab[(i + left) * cn + k] = j + k;
    }

    srcroi.width    *= cn;
    dstroi.width    *= cn;
    left            *= cn;
    right           *= cn;

    uchar* dstInner  = dst + dststep * top + left * elemSize;

    for (i = 0; i < srcroi.height; i++, dstInner += dststep, src += srcstep)
    {
        if (dstInner != src)
            memcpy(dstInner, src, srcroi.width * elemSize);

        if (intMode)
        {
            const int* isrc      = (int*)src;
            int*       idstInner = (int*)dstInner;
            for (j = 0; j < left; j++)
                idstInner[j - left] = isrc[tab[j]];
            for (j = 0; j < right; j++)
                idstInner[j + srcroi.width] = isrc[tab[j + left]];
        }
        else
        {
            for (j = 0; j < left; j++)
                dstInner[j - left] = src[tab[j]];
            for (j = 0; j < right; j++)
                dstInner[j + srcroi.width] = src[tab[j + left]];
        }
    }

    dstroi.width *= elemSize;

    for (i = 0; i < top; i++)
    {
        j = hl::borderInterpolate(i - top, srcroi.height, borderType);
        memcpy(dst + i * dststep, dst + (top + j) * dststep, dstroi.width);
    }

    dst += dststep * top;
    for (i = 0; i < bottom; i++)
    {
        j = hl::borderInterpolate(i + srcroi.height, srcroi.height, borderType);
        memcpy(dst + (i + srcroi.height) * dststep, dst + j * dststep, dstroi.width);
    }
}

void copyMakeConstBorder_8u(const uchar* src, size_t srcstep, hl::Size srcroi, uchar* dst, size_t dststep, hl::Size dstroi, int top, int left, int cn, const uchar* value)
{
    int                   i, j;
    hl::AutoBuffer<uchar> _constBuf(dstroi.width * cn);
    uchar*                constBuf = _constBuf.data();
    int                   right    = dstroi.width - srcroi.width - left;
    int                   bottom   = dstroi.height - srcroi.height - top;

    for (i = 0; i < dstroi.width; i++)
    {
        for (j = 0; j < cn; j++)
            constBuf[i * cn + j] = value[j];
    }

    srcroi.width    *= cn;
    dstroi.width    *= cn;
    left            *= cn;
    right           *= cn;

    uchar* dstInner  = dst + dststep * top + left;

    for (i = 0; i < srcroi.height; i++, dstInner += dststep, src += srcstep)
    {
        if (dstInner != src)
            memcpy(dstInner, src, srcroi.width);
        memcpy(dstInner - left, constBuf, left);
        memcpy(dstInner + srcroi.width, constBuf, right);
    }

    for (i = 0; i < top; i++)
        memcpy(dst + i * dststep, constBuf, dstroi.width);

    dst += (top + srcroi.height) * dststep;
    for (i = 0; i < bottom; i++)
        memcpy(dst + i * dststep, constBuf, dstroi.width);
}

}    // namespace

void hl::copyMakeBorder(const Mat& _src, Mat& _dst, int top, int bottom, int left, int right, int borderType, const Scalar& value)
{
    HL_Assert(top >= 0 && bottom >= 0 && left >= 0 && right >= 0 && _src.dims <= 2);

    Mat src  = _src;
    int type = src.type();

    if (src.isSubmatrix() && (borderType & BORDER_ISOLATED) == 0)
    {
        Size  wholeSize;
        Point ofs;
        src.locateROI(wholeSize, ofs);
        int dtop    = std::min(ofs.y, top);
        int dbottom = std::min(wholeSize.height - src.rows - ofs.y, bottom);
        int dleft   = std::min(ofs.x, left);
        int dright  = std::min(wholeSize.width - src.cols - ofs.x, right);
        src.adjustROI(dtop, dbottom, dleft, dright);
        top    -= dtop;
        left   -= dleft;
        bottom -= dbottom;
        right  -= dright;
    }

    _dst.create(src.rows + top + bottom, src.cols + left + right, type);
    Mat dst = _dst;

    if (top == 0 && left == 0 && bottom == 0 && right == 0)
    {
        if (src.data != dst.data || src.step != dst.step)
            src.copyTo(dst);
        return;
    }

    borderType &= ~BORDER_ISOLATED;

    if (borderType != BORDER_CONSTANT)
        copyMakeBorder_8u(src.ptr(), src.step, src.size(), dst.ptr(), dst.step, dst.size(), top, left, (int)src.elemSize(), borderType);
    else
    {
        int                cn = src.channels(), cn1 = cn;
        AutoBuffer<double> buf(cn);
        if (cn > 4)
        {
            HL_Assert(value[0] == value[1] && value[0] == value[2] && value[0] == value[3]);
            cn1 = 1;
        }
        scalarToRawData(value, buf.data(), HL_MAKETYPE(src.depth(), cn1), cn);
        copyMakeConstBorder_8u(src.ptr(), src.step, src.size(), dst.ptr(), dst.step, dst.size(), top, left, (int)src.elemSize(), (uchar*)buf.data());
    }
}
