#pragma once

#include "openHL/core.hxx"
#include <mutex>    // std::mutex, std::lock_guard

namespace hl
{

template <typename _Tp, size_t fixed_size = 1024 / sizeof(_Tp) + 8>
class AutoBuffer
{
public:
    typedef _Tp value_type;

    AutoBuffer();
    explicit AutoBuffer(size_t _size);
    AutoBuffer(const AutoBuffer<_Tp, fixed_size>& buf);
    ~AutoBuffer();

    AutoBuffer<_Tp, fixed_size>& operator=(const AutoBuffer<_Tp, fixed_size>& buf);


    void   allocate(size_t _size);
    void   deallocate();
    void   resize(size_t _size);
    size_t size() const;

    inline _Tp* data() { return ptr; }

    const inline _Tp* data() const { return ptr; }

    inline _Tp& operator[](size_t i)
    {
        HL_DbgCheckLT(i, sz, "out of range");
        return ptr[i];
    }

    const inline _Tp& operator[](size_t i) const
    {
        HL_DbgCheckLT(i, sz, "out of range");
        return ptr[i];
    }

protected:
    _Tp*   ptr;
    size_t sz;
    _Tp    buf[(fixed_size > 0) ? fixed_size : 1];
};

inline static size_t alignSize(size_t sz, int n)
{
    HL_DbgAssert((n & (n - 1)) == 0);    // n is a power of 2
    return (sz + n - 1) & -n;
}

inline static size_t getElemSize(int type) { return (size_t)HL_ELEM_SIZE(type); }

/////////////////////////////// Parallel Primitives //////////////////////////////////

class ParallelLoopBody
{
public:
    virtual ~ParallelLoopBody();
    virtual void operator()(const Range& range) const = 0;
};

void parallel_for_(const Range& range, const ParallelLoopBody& body, double nstripes = -1.);

/////////////////////////// Synchronization Primitives ///////////////////////////////

typedef std::recursive_mutex       Mutex;
typedef std::lock_guard<hl::Mutex> AutoLock;

/////////////////////////////// AutoBuffer implementation ////////////////////////////////////////

template <typename _Tp, size_t fixed_size>
inline AutoBuffer<_Tp, fixed_size>::AutoBuffer()
{
    ptr = buf;
    sz  = fixed_size;
}

template <typename _Tp, size_t fixed_size>
inline AutoBuffer<_Tp, fixed_size>::AutoBuffer(size_t _size)
{
    ptr = buf;
    sz  = fixed_size;
    allocate(_size);
}

template <typename _Tp, size_t fixed_size>
inline AutoBuffer<_Tp, fixed_size>::AutoBuffer(const AutoBuffer<_Tp, fixed_size>& abuf)
{
    ptr = buf;
    sz  = fixed_size;
    allocate(abuf.size());
    for (size_t i = 0; i < sz; i++)
        ptr[i] = abuf.ptr[i];
}

template <typename _Tp, size_t fixed_size>
inline AutoBuffer<_Tp, fixed_size>::~AutoBuffer()
{
    deallocate();
}

template <typename _Tp, size_t fixed_size>
inline AutoBuffer<_Tp, fixed_size>& AutoBuffer<_Tp, fixed_size>::operator=(const AutoBuffer<_Tp, fixed_size>& abuf)
{
    if (this != &abuf)
    {
        deallocate();
        allocate(abuf.size());
        for (size_t i = 0; i < sz; i++)
            ptr[i] = abuf.ptr[i];
    }
    return *this;
}

template <typename _Tp, size_t fixed_size>
inline void AutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
    if (_size <= sz)
    {
        sz = _size;
        return;
    }
    deallocate();
    sz = _size;
    if (_size > fixed_size)
    {
        ptr = new _Tp[_size];
    }
}

template <typename _Tp, size_t fixed_size>
inline void AutoBuffer<_Tp, fixed_size>::deallocate()
{
    if (ptr != buf)
    {
        delete[] ptr;
        ptr = buf;
        sz  = fixed_size;
    }
}

template <typename _Tp, size_t fixed_size>
inline void AutoBuffer<_Tp, fixed_size>::resize(size_t _size)
{
    if (_size <= sz)
    {
        sz = _size;
        return;
    }
    size_t i, prevsize = sz, minsize = MIN(prevsize, _size);
    _Tp*   prevptr = ptr;

    ptr            = _size > fixed_size ? new _Tp[_size] : buf;
    sz             = _size;

    if (ptr != prevptr)
        for (i = 0; i < minsize; i++)
            ptr[i] = prevptr[i];
    for (i = prevsize; i < _size; i++)
        ptr[i] = _Tp();

    if (prevptr != buf)
        delete[] prevptr;
}

template <typename _Tp>
inline static _Tp* alignPtr(_Tp* ptr, int n = (int)sizeof(_Tp))
{
    HL_DbgAssert((n & (n - 1)) == 0);    // n is a power of 2
    return (_Tp*)(((size_t)ptr + n - 1) & -n);
}

template <typename _Tp, size_t fixed_size>
inline size_t AutoBuffer<_Tp, fixed_size>::size() const
{
    return sz;
}
}    // namespace hl