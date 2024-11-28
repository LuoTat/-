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

}    // namespace hl
