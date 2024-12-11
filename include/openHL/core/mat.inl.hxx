#pragma once

namespace hl
{

//////////////////////////////////////////// Mat //////////////////////////////////////////

inline Mat Mat::row(int y) const
{
    return Mat(*this, Range(y, y + 1), Range::all());
}

inline Mat Mat::col(int x) const
{
    return Mat(*this, Range::all(), Range(x, x + 1));
}

inline Mat Mat::rowRange(int startrow, int endrow) const
{
    return Mat(*this, Range(startrow, endrow), Range::all());
}

inline Mat Mat::rowRange(const Range& r) const
{
    return Mat(*this, r, Range::all());
}

inline Mat Mat::colRange(int startcol, int endcol) const
{
    return Mat(*this, Range::all(), Range(startcol, endcol));
}

inline Mat Mat::colRange(const Range& r) const
{
    return Mat(*this, Range::all(), r);
}

inline Mat Mat::operator()(Range _rowRange, Range _colRange) const
{
    return Mat(*this, _rowRange, _colRange);
}

inline Mat Mat::operator()(const Rect& roi) const
{
    return Mat(*this, roi);
}

inline Mat Mat::operator()(const Range* ranges) const
{
    return Mat(*this, ranges);
}

inline Mat Mat::operator()(const std::vector<Range>& ranges) const
{
    return Mat(*this, ranges);
}

inline bool Mat::isContinuous() const
{
    return (flags & CONTINUOUS_FLAG) != 0;
}

inline bool Mat::isSubmatrix() const
{
    return (flags & SUBMATRIX_FLAG) != 0;
}

inline size_t Mat::elemSize() const
{
    size_t res = dims > 0 ? step.p[dims - 1] : 0;
    HL_DbgAssert(res != 0);
    return res;
}

inline size_t Mat::elemSize1() const
{
    return HL_ELEM_SIZE1(flags);
}

inline int Mat::type() const
{
    return HL_MAT_TYPE(flags);
}

inline int Mat::depth() const
{
    return HL_MAT_DEPTH(flags);
}

inline int Mat::channels() const
{
    return HL_MAT_CN(flags);
}

inline uchar* Mat::ptr(int y)
{
    HL_DbgAssert(y == 0 || (data && dims >= 1 && (uint)y < (uint)size.p[0]));
    return data + step.p[0] * y;
}

const inline uchar* Mat::ptr(int y) const
{
    HL_DbgAssert(y == 0 || (data && dims >= 1 && (uint)y < (uint)size.p[0]));
    return data + step.p[0] * y;
}

template <typename _Tp>
inline _Tp* Mat::ptr(int y)
{
    HL_DbgAssert(y == 0 || (data && dims >= 1 && (uint)y < (uint)size.p[0]));
    return (_Tp*)(data + step.p[0] * y);
}

template <typename _Tp>
const inline _Tp* Mat::ptr(int y) const
{
    HL_DbgAssert(y == 0 || (data && dims >= 1 && (uint)y < (uint)size.p[0]));
    return (const _Tp*)(data + step.p[0] * y);
}

inline uchar* Mat::ptr(int i0, int i1)
{
    HL_DbgAssert(dims >= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    return data + i0 * step.p[0] + i1 * step.p[1];
}

const inline uchar* Mat::ptr(int i0, int i1) const
{
    HL_DbgAssert(dims >= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    return data + i0 * step.p[0] + i1 * step.p[1];
}

template <typename _Tp>
inline _Tp* Mat::ptr(int i0, int i1)
{
    HL_DbgAssert(dims >= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    return (_Tp*)(data + i0 * step.p[0] + i1 * step.p[1]);
}

template <typename _Tp>
const inline _Tp* Mat::ptr(int i0, int i1) const
{
    HL_DbgAssert(dims >= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    return (const _Tp*)(data + i0 * step.p[0] + i1 * step.p[1]);
}

inline uchar* Mat::ptr(int i0, int i1, int i2)
{
    HL_DbgAssert(dims >= 3);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    HL_DbgAssert((uint)i2 < (uint)size.p[2]);
    return data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2];
}

const inline uchar* Mat::ptr(int i0, int i1, int i2) const
{
    HL_DbgAssert(dims >= 3);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    HL_DbgAssert((uint)i2 < (uint)size.p[2]);
    return data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2];
}

template <typename _Tp>
inline _Tp* Mat::ptr(int i0, int i1, int i2)
{
    HL_DbgAssert(dims >= 3);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    HL_DbgAssert((uint)i2 < (uint)size.p[2]);
    return (_Tp*)(data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2]);
}

template <typename _Tp>
const inline _Tp* Mat::ptr(int i0, int i1, int i2) const
{
    HL_DbgAssert(dims >= 3);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)i1 < (uint)size.p[1]);
    HL_DbgAssert((uint)i2 < (uint)size.p[2]);
    return (const _Tp*)(data + i0 * step.p[0] + i1 * step.p[1] + i2 * step.p[2]);
}

inline uchar* Mat::ptr(const int* idx)
{
    int    i, d = dims;
    uchar* p = data;
    HL_DbgAssert(d >= 1 && p);
    for (i = 0; i < d; i++)
    {
        HL_DbgAssert((uint)idx[i] < (uint)size.p[i]);
        p += idx[i] * step.p[i];
    }
    return p;
}

const inline uchar* Mat::ptr(const int* idx) const
{
    int    i, d = dims;
    uchar* p = data;
    HL_DbgAssert(d >= 1 && p);
    for (i = 0; i < d; i++)
    {
        HL_DbgAssert((uint)idx[i] < (uint)size.p[i]);
        p += idx[i] * step.p[i];
    }
    return p;
}

template <typename _Tp>
inline _Tp* Mat::ptr(const int* idx)
{
    int    i, d = dims;
    uchar* p = data;
    HL_DbgAssert(d >= 1 && p);
    for (i = 0; i < d; i++)
    {
        HL_DbgAssert((uint)idx[i] < (uint)size.p[i]);
        p += idx[i] * step.p[i];
    }
    return (_Tp*)p;
}

template <typename _Tp>
const inline _Tp* Mat::ptr(const int* idx) const
{
    int    i, d = dims;
    uchar* p = data;
    HL_DbgAssert(d >= 1 && p);
    for (i = 0; i < d; i++)
    {
        HL_DbgAssert((uint)idx[i] < (uint)size.p[i]);
        p += idx[i] * step.p[i];
    }
    return (const _Tp*)p;
}

template <typename _Tp>
inline _Tp& Mat::at(int i0)
{
    HL_DbgAssert(dims <= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)(size.p[0] * size.p[1]));
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    if (size.p[0] == 1)
        return ((_Tp*)data)[i0];
    if (size.p[1] == 1)
        return *(_Tp*)(data + step.p[0] * i0);
    int i = i0 / cols, j = i0 - i * cols;
    return ((_Tp*)(data + step.p[0] * i))[j];
}

template <typename _Tp>
const inline _Tp& Mat::at(int i0) const
{
    HL_DbgAssert(dims <= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)(size.p[0] * size.p[1]));
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    if (size.p[0] == 1)
        return ((const _Tp*)data)[i0];
    if (size.p[1] == 1)
        return *(const _Tp*)(data + step.p[0] * i0);
    int i = i0 / cols, j = i0 - i * cols;
    return ((const _Tp*)(data + step.p[0] * i))[j];
}

template <typename _Tp>
inline _Tp& Mat::at(int i0, int i1)
{
    HL_DbgAssert(dims <= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)(i1 * DataType<_Tp>::channels) < (uint)(size.p[1] * channels()));
    HL_DbgAssert(HL_ELEM_SIZE1(traits::Depth<_Tp>::value) == elemSize1());
    return ((_Tp*)(data + step.p[0] * i0))[i1];
}

template <typename _Tp>
const inline _Tp& Mat::at(int i0, int i1) const
{
    HL_DbgAssert(dims <= 2);
    HL_DbgAssert(data);
    HL_DbgAssert((uint)i0 < (uint)size.p[0]);
    HL_DbgAssert((uint)(i1 * DataType<_Tp>::channels) < (uint)(size.p[1] * channels()));
    HL_DbgAssert(HL_ELEM_SIZE1(traits::Depth<_Tp>::value) == elemSize1());
    return ((const _Tp*)(data + step.p[0] * i0))[i1];
}

template <typename _Tp>
inline _Tp& Mat::at(int i0, int i1, int i2)
{
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    return *(_Tp*)ptr(i0, i1, i2);
}

template <typename _Tp>
const inline _Tp& Mat::at(int i0, int i1, int i2) const
{
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    return *(const _Tp*)ptr(i0, i1, i2);
}

template <typename _Tp>
inline _Tp& Mat::at(const int* idx)
{
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    return *(_Tp*)ptr(idx);
}

template <typename _Tp>
const inline _Tp& Mat::at(const int* idx) const
{
    HL_DbgAssert(elemSize() == sizeof(_Tp));
    return *(const _Tp*)ptr(idx);
}

///////////////////////////// MatSize ////////////////////////////

inline MatSize::MatSize(int* _p) noexcept:
    p(_p) {}

inline int MatSize::dims() const noexcept
{
    return p[-1];
}

inline Size MatSize::operator()() const
{
    HL_DbgAssert(dims() <= 2);
    return Size(p[1], p[0]);
}

const inline int& MatSize::operator[](int i) const
{
    HL_DbgAssert(i < dims());
    HL_DbgAssert(i >= 0);

    return p[i];
}

inline int& MatSize::operator[](int i)
{
    HL_DbgAssert(i < dims());
    HL_DbgAssert(i >= 0);

    return p[i];
}

inline MatSize::operator const int*() const noexcept
{
    return p;
}

inline bool MatSize::operator!=(const MatSize& sz) const noexcept
{
    return !(*this == sz);
}

///////////////////////////// MatStep ////////////////////////////

inline MatStep::MatStep() noexcept
{
    p    = buf;
    p[0] = p[1] = 0;
}

inline MatStep::MatStep(size_t s) noexcept
{
    p    = buf;
    p[0] = s;
    p[1] = 0;
}

const inline size_t& MatStep::operator[](int i) const noexcept
{
    return p[i];
}

inline size_t& MatStep::operator[](int i) noexcept
{
    return p[i];
}

inline MatStep::operator size_t() const
{
    HL_DbgAssert(p == buf);
    return buf[0];
}

inline MatStep& MatStep::operator=(size_t s)
{
    HL_DbgAssert(p == buf);
    buf[0] = s;
    return *this;
}

inline bool UMatData::hostCopyObsolete() const { return (flags & HOST_COPY_OBSOLETE) != 0; }

inline bool UMatData::deviceCopyObsolete() const { return (flags & DEVICE_COPY_OBSOLETE) != 0; }

inline bool UMatData::deviceMemMapped() const { return (flags & DEVICE_MEM_MAPPED) != 0; }

inline bool UMatData::copyOnMap() const { return (flags & COPY_ON_MAP) != 0; }

inline bool UMatData::tempUMat() const { return (flags & TEMP_UMAT) != 0; }

inline bool UMatData::tempCopiedUMat() const { return (flags & TEMP_COPIED_UMAT) == TEMP_COPIED_UMAT; }

inline void UMatData::markHostCopyObsolete(bool flag)
{
    if (flag)
        flags |= HOST_COPY_OBSOLETE;
    else
        flags &= ~HOST_COPY_OBSOLETE;
}

inline void UMatData::markDeviceCopyObsolete(bool flag)
{
    if (flag)
        flags |= DEVICE_COPY_OBSOLETE;
    else
        flags &= ~DEVICE_COPY_OBSOLETE;
}

inline void UMatData::markDeviceMemMapped(bool flag)
{
    if (flag)
        flags |= DEVICE_MEM_MAPPED;
    else
        flags &= ~DEVICE_MEM_MAPPED;
}

}    // namespace hl