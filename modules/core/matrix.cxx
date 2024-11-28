#include "precomp.hxx"
#include "bufferpool.impl.hxx"

namespace hl
{

void MatAllocator::map(UMatData*, AccessFlag) const
{
}

void MatAllocator::unmap(UMatData* u) const
{
    if (u->urefcount == 0 && u->refcount == 0)
    {
        deallocate(u);
    }
}

void MatAllocator::download(UMatData* u, void* dstptr, int dims, const size_t sz[], const size_t srcofs[], const size_t srcstep[], const size_t dststep[]) const
{
    if (!u)
        return;
    int    isz[HL_MAX_DIM];
    uchar* srcptr = u->data;
    for (int i = 0; i < dims; i++)
    {
        HL_Assert(sz[i] <= (size_t)INT_MAX);
        if (sz[i] == 0)
            return;
        if (srcofs)
            srcptr += srcofs[i] * (i <= dims - 2 ? srcstep[i] : 1);
        isz[i] = (int)sz[i];
    }

    Mat src(dims, isz, HL_8U, srcptr, srcstep);
    Mat dst(dims, isz, HL_8U, dstptr, dststep);

    const Mat*      arrays[] = {&src, &dst};
    uchar*          ptrs[2];
    NAryMatIterator it(arrays, ptrs, 2);
    size_t          planesz = it.size;

    for (size_t j = 0; j < it.nplanes; j++, ++it)
        memcpy(ptrs[1], ptrs[0], planesz);
}

void MatAllocator::upload(UMatData* u, const void* srcptr, int dims, const size_t sz[], const size_t dstofs[], const size_t dststep[], const size_t srcstep[]) const
{
    if (!u)
        return;
    int    isz[HL_MAX_DIM];
    uchar* dstptr = u->data;
    for (int i = 0; i < dims; i++)
    {
        HL_Assert(sz[i] <= (size_t)INT_MAX);
        if (sz[i] == 0)
            return;
        if (dstofs)
            dstptr += dstofs[i] * (i <= dims - 2 ? dststep[i] : 1);
        isz[i] = (int)sz[i];
    }

    Mat src(dims, isz, HL_8U, (void*)srcptr, srcstep);
    Mat dst(dims, isz, HL_8U, dstptr, dststep);

    const Mat*      arrays[] = {&src, &dst};
    uchar*          ptrs[2];
    NAryMatIterator it(arrays, ptrs, 2);
    size_t          planesz = it.size;

    for (size_t j = 0; j < it.nplanes; j++, ++it)
        memcpy(ptrs[1], ptrs[0], planesz);
}

void MatAllocator::copy(UMatData* usrc, UMatData* udst, int dims, const size_t sz[], const size_t srcofs[], const size_t srcstep[], const size_t dstofs[], const size_t dststep[], bool /*sync*/) const
{
    if (!usrc || !udst)
        return;
    int    isz[HL_MAX_DIM];
    uchar* srcptr = usrc->data;
    uchar* dstptr = udst->data;
    for (int i = 0; i < dims; i++)
    {
        HL_Assert(sz[i] <= (size_t)INT_MAX);
        if (sz[i] == 0)
            return;
        if (srcofs)
            srcptr += srcofs[i] * (i <= dims - 2 ? srcstep[i] : 1);
        if (dstofs)
            dstptr += dstofs[i] * (i <= dims - 2 ? dststep[i] : 1);
        isz[i] = (int)sz[i];
    }

    Mat src(dims, isz, HL_8U, srcptr, srcstep);
    Mat dst(dims, isz, HL_8U, dstptr, dststep);

    const Mat*      arrays[] = {&src, &dst};
    uchar*          ptrs[2];
    NAryMatIterator it(arrays, ptrs, 2);
    size_t          planesz = it.size;

    for (size_t j = 0; j < it.nplanes; j++, ++it)
        memcpy(ptrs[1], ptrs[0], planesz);
}

BufferPoolController* MatAllocator::getBufferPoolController(const char* id) const
{
    HL_UNUSED(id);
    static DummyBufferPoolController dummy;
    return &dummy;
}

class StdMatAllocator final: public MatAllocator
{
public:
    UMatData* allocate(int dims, const int* sizes, int type, void* data0, size_t* step, AccessFlag /*flags*/, UMatUsageFlags /*usageFlags*/) const override
    {
        size_t total = HL_ELEM_SIZE(type);
        for (int i = dims - 1; i >= 0; i--)
        {
            if (step)
            {
                if (data0 && step[i] != HL_AUTOSTEP)
                {
                    HL_Assert(total <= step[i]);
                    total = step[i];
                }
                else
                    step[i] = total;
            }
            total *= sizes[i];
        }
        uchar*    data = data0 ? (uchar*)data0 : (uchar*)fastMalloc(total);
        UMatData* u    = new UMatData(this);
        u->data = u->origdata = data;
        u->size               = total;
        if (data0)
            u->flags |= UMatData::USER_ALLOCATED;

        return u;
    }

    bool allocate(UMatData* u, AccessFlag /*accessFlags*/, UMatUsageFlags /*usageFlags*/) const override
    {
        if (!u) return false;
        return true;
    }

    void deallocate(UMatData* u) const override
    {
        if (!u)
            return;

        HL_Assert(u->urefcount == 0);
        HL_Assert(u->refcount == 0);
        if (!(u->flags & UMatData::USER_ALLOCATED))
        {
            fastFree(u->origdata);
            u->origdata = 0;
        }
        delete u;
    }
};

static MatAllocator*& getDefaultAllocatorMatRef()
{
    static MatAllocator* g_matAllocator = Mat::getStdAllocator();
    return g_matAllocator;
}

MatAllocator* Mat::getDefaultAllocator()
{
    return getDefaultAllocatorMatRef();
}

void Mat::setDefaultAllocator(MatAllocator* allocator)
{
    getDefaultAllocatorMatRef() = allocator;
}

MatAllocator* Mat::getStdAllocator()
{
    HL_SINGLETON_LAZY_INIT(MatAllocator, new StdMatAllocator())
}

//==================================================================================================

bool MatSize::operator==(const MatSize& sz) const noexcept
{
    int d   = dims();
    int dsz = sz.dims();
    if (d != dsz)
        return false;
    if (d == 2)
        return p[0] == sz.p[0] && p[1] == sz.p[1];

    for (int i = 0; i < d; i++)
        if (p[i] != sz.p[i])
            return false;
    return true;
}

void setSize(Mat& m, int _dims, const int* _sz, const size_t* _steps, bool autoSteps)
{
    HL_Assert(0 <= _dims && _dims <= HL_MAX_DIM);
    if (m.dims != _dims)
    {
        if (m.step.p != m.step.buf)
        {
            fastFree(m.step.p);
            m.step.p = m.step.buf;
            m.size.p = &m.rows;
        }
        if (_dims > 2)
        {
            m.step.p     = (size_t*)fastMalloc(_dims * sizeof(m.step.p[0]) + (_dims + 1) * sizeof(m.size.p[0]));
            m.size.p     = (int*)(m.step.p + _dims) + 1;
            m.size.p[-1] = _dims;
            m.rows = m.cols = -1;
        }
    }

    m.dims = _dims;
    if (!_sz)
        return;

    size_t esz = HL_ELEM_SIZE(m.flags), esz1 = HL_ELEM_SIZE1(m.flags), total = esz;
    for (int i = _dims - 1; i >= 0; i--)
    {
        int s = _sz[i];
        HL_Assert(s >= 0);
        m.size.p[i] = s;

        if (_steps)
        {
            if (i < _dims - 1)
            {
                if (_steps[i] % esz1 != 0)
                {
                    HL_Error_(Error::BadStep, ("Step {} for dimension {} must be a multiple of esz1 {}", _steps[i], i, esz1));
                }

                m.step.p[i] = _steps[i];
            }
            else
            {
                m.step.p[i] = esz;
            }
        }
        else if (autoSteps)
        {
            m.step.p[i]   = total;
            uint64 total1 = (uint64)total * s;
            if ((uint64)total1 != (size_t)total1)
                HL_Error(HL_StsOutOfRange, "The total matrix size does not fit to \"size_t\" type");
            total = (size_t)total1;
        }
    }

    if (_dims == 1)
    {
        m.dims    = 2;
        m.cols    = 1;
        m.step[1] = esz;
    }
}

int updateContinuityFlag(int flags, int dims, const int* size, const size_t* step)
{
    int i, j;
    for (i = 0; i < dims; i++)
    {
        if (size[i] > 1)
            break;
    }

    uint64 t = (uint64)size[std::min(i, dims - 1)] * HL_MAT_CN(flags);
    for (j = dims - 1; j > i; j--)
    {
        t *= size[j];
        if (step[j] * size[j] < step[j - 1])
            break;
    }

    if (j <= i && t == (uint64)(int)t)
        return flags | Mat::CONTINUOUS_FLAG;
    return flags & ~Mat::CONTINUOUS_FLAG;
}

void finalizeHdr(Mat& m)
{
    m.updateContinuityFlag();
    int d = m.dims;
    if (d > 2)
        m.rows = m.cols = -1;
    if (m.u)
        m.datastart = m.data = m.u->data;
    if (m.data)
    {
        m.datalimit = m.datastart + m.size[0] * m.step[0];
        if (m.size[0] > 0)
        {
            m.dataend = m.ptr() + m.size[d - 1] * m.step[d - 1];
            for (int i = 0; i < d - 1; i++)
                m.dataend += (m.size[i] - 1) * m.step[i];
        }
        else
            m.dataend = m.datalimit;
    }
    else
        m.dataend = m.datalimit = 0;
}

//======================================= Mat ======================================================

Mat::Mat() noexcept
    :
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{}

Mat::Mat(const Mat& m):
    flags(m.flags), dims(m.dims), rows(m.rows), cols(m.cols), data(m.data), datastart(m.datastart), dataend(m.dataend), datalimit(m.datalimit), allocator(m.allocator), u(m.u), size(&rows), step(0)
{
    if (u)
        HL_XADD(&u->refcount, 1);
    if (m.dims <= 2)
    {
        step[0] = m.step[0];
        step[1] = m.step[1];
    }
    else
    {
        dims = 0;
        copySize(m);
    }
}

Mat::Mat(Mat&& m):
    flags(m.flags), dims(m.dims), rows(m.rows), cols(m.cols), data(m.data), datastart(m.datastart), dataend(m.dataend), datalimit(m.datalimit), allocator(m.allocator), u(m.u), size(&rows)
{
    if (m.dims <= 2)    // move new step/size info
    {
        step[0] = m.step[0];
        step[1] = m.step[1];
    }
    else
    {
        HL_Assert(m.step.p != m.step.buf);
        step.p   = m.step.p;
        size.p   = m.size.p;
        m.step.p = m.step.buf;
        m.size.p = &m.rows;
    }
    m.flags = MAGIC_VAL;
    m.dims = m.rows = m.cols = 0;
    m.data                   = NULL;
    m.datastart              = NULL;
    m.dataend                = NULL;
    m.datalimit              = NULL;
    m.allocator              = NULL;
    m.u                      = NULL;
}

Mat::Mat(int _rows, int _cols, int _type):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_rows, _cols, _type);
}

Mat::Mat(int _rows, int _cols, int _type, const Scalar& _s):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_rows, _cols, _type);
    *this = _s;
}

Mat::Mat(Size _sz, int _type):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_sz.height, _sz.width, _type);
}

Mat::Mat(Size _sz, int _type, const Scalar& _s):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_sz.height, _sz.width, _type);
    *this = _s;
}

Mat::Mat(int _dims, const int* _sz, int _type):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_dims, _sz, _type);
}

Mat::Mat(int _dims, const int* _sz, int _type, const Scalar& _s):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows), step(0)
{
    create(_dims, _sz, _type);
    *this = _s;
}

Mat::Mat(int _rows, int _cols, int _type, void* _data, size_t _step):
    flags(MAGIC_VAL + (_type & TYPE_MASK)), dims(2), rows(_rows), cols(_cols), data((uchar*)_data), datastart((uchar*)_data), dataend(0), datalimit(0), allocator(0), u(0), size(&rows)
{
    HL_Assert(total() == 0 || data != NULL);

    size_t esz = HL_ELEM_SIZE(_type), esz1 = HL_ELEM_SIZE1(_type);
    size_t minstep = cols * esz;
    if (_step == AUTO_STEP)
    {
        _step = minstep;
    }
    else
    {
        HL_Assert(_step >= minstep);

        if (_step % esz1 != 0)
        {
            HL_Error(Error::BadStep, "Step must be a multiple of esz1");
        }
    }
    step[0]   = _step;
    step[1]   = esz;
    datalimit = datastart + _step * rows;
    dataend   = datalimit - _step + minstep;
    updateContinuityFlag();
}

Mat::Mat(Size _sz, int _type, void* _data, size_t _step):
    flags(MAGIC_VAL + (_type & TYPE_MASK)), dims(2), rows(_sz.height), cols(_sz.width), data((uchar*)_data), datastart((uchar*)_data), dataend(0), datalimit(0), allocator(0), u(0), size(&rows)
{
    HL_Assert(total() == 0 || data != NULL);

    size_t esz = HL_ELEM_SIZE(_type), esz1 = HL_ELEM_SIZE1(_type);
    size_t minstep = cols * esz;
    if (_step == AUTO_STEP)
    {
        _step = minstep;
    }
    else
    {
        HL_CheckGE(_step, minstep, "");

        if (_step % esz1 != 0)
        {
            HL_Error(Error::BadStep, "Step must be a multiple of esz1");
        }
    }
    step[0]   = _step;
    step[1]   = esz;
    datalimit = datastart + _step * rows;
    dataend   = datalimit - _step + minstep;
    updateContinuityFlag();
}

Mat::Mat(int _dims, const int* _sizes, int _type, void* _data, const size_t* _steps):
    flags(MAGIC_VAL), dims(0), rows(0), cols(0), data(0), datastart(0), dataend(0), datalimit(0), allocator(0), u(0), size(&rows)
{
    flags     |= HL_MAT_TYPE(_type);
    datastart = data = (uchar*)_data;
    setSize(*this, _dims, _sizes, _steps, true);
    finalizeHdr(*this);
}

Mat::~Mat()
{
    release();
    if (step.p != step.buf)
        free(step.p);
}

Mat& Mat::operator=(const Mat& m)
{
    if (this != &m)
    {
        if (m.u)
            HL_XADD(&m.u->refcount, 1);
        release();
        flags = m.flags;
        if (dims <= 2 && m.dims <= 2)
        {
            dims    = m.dims;
            rows    = m.rows;
            cols    = m.cols;
            step[0] = m.step[0];
            step[1] = m.step[1];
        }
        else
            copySize(m);
        data      = m.data;
        datastart = m.datastart;
        dataend   = m.dataend;
        datalimit = m.datalimit;
        allocator = m.allocator;
        u         = m.u;
    }
    return *this;
}

Mat& Mat::operator=(Mat&& m)
{
    if (this == &m)
        return *this;

    release();
    flags     = m.flags;
    dims      = m.dims;
    rows      = m.rows;
    cols      = m.cols;
    data      = m.data;
    datastart = m.datastart;
    dataend   = m.dataend;
    datalimit = m.datalimit;
    allocator = m.allocator;
    u         = m.u;
    if (step.p != step.buf)    // release self step/size
    {
        fastFree(step.p);
        step.p = step.buf;
        size.p = &rows;
    }
    if (m.dims <= 2)           // move new step/size info
    {
        step[0] = m.step[0];
        step[1] = m.step[1];
    }
    else
    {
        HL_Assert(m.step.p != m.step.buf);
        step.p   = m.step.p;
        size.p   = m.size.p;
        m.step.p = m.step.buf;
        m.size.p = &m.rows;
    }
    m.flags = MAGIC_VAL;
    m.dims = m.rows = m.cols = 0;
    m.data                   = NULL;
    m.datastart              = NULL;
    m.dataend                = NULL;
    m.datalimit              = NULL;
    m.allocator              = NULL;
    m.u                      = NULL;
    return *this;
}

Mat Mat::reshape(int new_cn, int new_rows) const
{
    int cn  = channels();
    Mat hdr = *this;

    if (dims > 2)
    {
        if (new_rows == 0 && new_cn != 0 && size[dims - 1] * cn % new_cn == 0)
        {
            hdr.flags          = (hdr.flags & ~HL_MAT_CN_MASK) | ((new_cn - 1) << HL_CN_SHIFT);
            hdr.step[dims - 1] = HL_ELEM_SIZE(hdr.flags);
            hdr.size[dims - 1] = hdr.size[dims - 1] * cn / new_cn;
            return hdr;
        }
        if (new_rows > 0)
        {
            int sz[] = {new_rows, (int)(total() / new_rows)};
            return reshape(new_cn, 2, sz);
        }
    }

    HL_Assert(dims <= 2);

    if (new_cn == 0)
        new_cn = cn;

    int total_width = cols * cn;

    if ((new_cn > total_width || total_width % new_cn != 0) && new_rows == 0)
        new_rows = rows * total_width / new_cn;

    if (new_rows != 0 && new_rows != rows)
    {
        int total_size = total_width * rows;
        if (!isContinuous())
            HL_Error(HL_BadStep, "The matrix is not continuous, thus its number of rows can not be changed");

        if ((unsigned)new_rows > (unsigned)total_size)
            HL_Error(HL_StsOutOfRange, "Bad new number of rows");

        total_width = total_size / new_rows;

        if (total_width * new_rows != total_size)
            HL_Error(HL_StsBadArg, "The total number of matrix elements is not divisible by the new number of rows");

        hdr.rows    = new_rows;
        hdr.step[0] = total_width * elemSize1();
    }

    int new_width = total_width / new_cn;

    if (new_width * new_cn != total_width)
        HL_Error(HL_BadNumChannels, "The total width is not divisible by the new number of channels");

    hdr.cols    = new_width;
    hdr.flags   = (hdr.flags & ~HL_MAT_CN_MASK) | ((new_cn - 1) << HL_CN_SHIFT);
    hdr.step[1] = HL_ELEM_SIZE(hdr.flags);
    return hdr;
}

Mat Mat::reshape(int _cn, int _newndims, const int* _newsz) const
{
    if (_newndims == dims)
    {
        if (_newsz == 0)
            return reshape(_cn);
        if (_newndims == 2)
            return reshape(_cn, _newsz[0]);
    }

    if (isContinuous())
    {
        HL_Assert(_cn >= 0 && _newndims > 0 && _newndims <= HL_MAX_DIM && _newsz);

        if (_cn == 0)
            _cn = this->channels();
        else
            HL_Assert(_cn <= HL_CN_MAX);

        size_t total_elem1_ref = this->total() * this->channels();
        size_t total_elem1     = _cn;

        AutoBuffer<int, 4> newsz_buf((size_t)_newndims);

        for (int i = 0; i < _newndims; i++)
        {
            HL_Assert(_newsz[i] >= 0);

            if (_newsz[i] > 0)
                newsz_buf[i] = _newsz[i];
            else if (i < dims)
                newsz_buf[i] = this->size[i];
            else
                HL_Error(HL_StsOutOfRange, "Copy dimension (which has zero size) is not present in source matrix");

            total_elem1 *= (size_t)newsz_buf[i];
        }

        if (total_elem1 != total_elem1_ref)
            HL_Error(HL_StsUnmatchedSizes, "Requested and source matrices have different count of elements");

        Mat hdr   = *this;
        hdr.flags = (hdr.flags & ~HL_MAT_CN_MASK) | ((_cn - 1) << HL_CN_SHIFT);
        setSize(hdr, _newndims, newsz_buf.data(), NULL, true);

        return hdr;
    }

    HL_Error(HL_StsNotImplemented, "Reshaping of n-dimensional non-continuous matrices is not supported yet");
    // TBD
}

void Mat::create(int _rows, int _cols, int _type)
{
    _type &= TYPE_MASK;
    if (dims <= 2 && rows == _rows && cols == _cols && type() == _type && data)
        return;
    int sz[] = {_rows, _cols};
    create(2, sz, _type);
}

void Mat::create(Size _sz, int _type)
{
    create(_sz.height, _sz.width, _type);
}

void Mat::create(int d, const int* _sizes, int _type)
{
    int i;
    HL_Assert(0 <= d && d <= HL_MAX_DIM && _sizes);
    _type = HL_MAT_TYPE(_type);

    if (data && (d == dims || (d == 1 && dims <= 2)) && _type == type())
    {
        if (dims == 1 && (d == 1 && _sizes[0] == size[0]))
            return;
        if (d == 2 && rows == _sizes[0] && cols == _sizes[1])
            return;
        for (i = 0; i < d; i++)
            if (size[i] != _sizes[i])
                break;
        if (i == d && (d > 1 || size[1] == 1))
            return;
    }

    int _sizes_backup[HL_MAX_DIM];    // #5991
    if (_sizes == (this->size.p))
    {
        for (i = 0; i < d; i++)
            _sizes_backup[i] = _sizes[i];
        _sizes = _sizes_backup;
    }

    release();
    if (d == 0)
        return;
    flags = (_type & HL_MAT_TYPE_MASK) | MAGIC_VAL;
    setSize(*this, d, _sizes, 0, true);

    if (total() > 0)
    {
        MatAllocator *a = allocator, *a0 = getDefaultAllocator();
        if (!a)
            a = a0;
        try
        {
            u = a->allocate(dims, size, _type, 0, step.p, ACCESS_RW /* ignored */, USAGE_DEFAULT);
            HL_Assert(u != 0);
        }
        catch (...)
        {
            if (a == a0)
                throw;
            u = a0->allocate(dims, size, _type, 0, step.p, ACCESS_RW /* ignored */, USAGE_DEFAULT);
            HL_Assert(u != 0);
        }
        HL_Assert(step[dims - 1] == (size_t)HL_ELEM_SIZE(flags));
    }

    addref();
    finalizeHdr(*this);
}

void Mat::addref()
{
    if (u)
        HL_XADD(&u->refcount, 1);
}

void Mat::deallocate()
{
    if (u)
    {
        UMatData* u_ = u;
        u            = NULL;
        (u_->currAllocator ? u_->currAllocator : allocator ? allocator
                                                           : getDefaultAllocator())
            ->unmap(u_);
    }
}

void Mat::release()
{
    if (u && HL_XADD(&u->refcount, -1) == 1)
        deallocate();
    u         = NULL;
    datastart = dataend = datalimit = data = 0;
    for (int i = 0; i < dims; i++)
        size.p[i] = 0;
}

size_t Mat::step1(int i) const
{
    return step.p[i] / elemSize1();
}

bool Mat::empty() const
{
    return data == 0 || total() == 0 || dims == 0;
}

size_t Mat::total() const
{
    if (dims <= 2)
        return (size_t)rows * cols;
    size_t p = 1;
    for (int i = 0; i < dims; i++)
        p *= size[i];
    return p;
}

size_t Mat::total(int startDim, int endDim) const
{
    HL_Assert(0 <= startDim && startDim <= endDim);
    size_t p       = 1;
    int    endDim_ = endDim <= dims ? endDim : dims;
    for (int i = startDim; i < endDim_; i++)
        p *= size[i];
    return p;
}

void Mat::copySize(const Mat& m)
{
    setSize(*this, m.dims, 0, 0);
    for (int i = 0; i < dims; i++)
    {
        size[i] = m.size[i];
        step[i] = m.step[i];
    }
}

void Mat::updateContinuityFlag()
{
    flags = hl::updateContinuityFlag(flags, dims, size.p, step.p);
}

inline static Size getContinuousSize_(int flags, int cols, int rows, int widthScale)
{
    int64 sz               = (int64)cols * rows * widthScale;
    bool  has_int_overflow = sz >= INT_MAX;
    bool  isContiguous     = (flags & Mat::CONTINUOUS_FLAG) != 0;
    return (isContiguous && !has_int_overflow) ? Size((int)sz, 1) : Size(cols * widthScale, rows);
}

Size getContinuousSize2D(Mat& m1, int widthScale)
{
    HL_CheckLE(m1.dims, 2, "");
    return getContinuousSize_(m1.flags, m1.cols, m1.rows, widthScale);
}

Size getContinuousSize2D(Mat& m1, Mat& m2, int widthScale)
{
    HL_CheckLE(m1.dims, 2, "");
    HL_CheckLE(m2.dims, 2, "");
    const Size sz1 = m1.size();
    if (sz1 != m2.size())    // reshape all matrixes to the same size (#4159)
    {
        size_t total_sz = m1.total();
        HL_CheckEQ(total_sz, m2.total(), "");
        bool is_m1_vector = m1.cols == 1 || m1.rows == 1;
        bool is_m2_vector = m2.cols == 1 || m2.rows == 1;
        HL_Assert(is_m1_vector);
        HL_Assert(is_m2_vector);
        int  total            = (int)total_sz;    // vector-column
        bool isContiguous     = ((m1.flags & m2.flags) & Mat::CONTINUOUS_FLAG) != 0;
        bool has_int_overflow = ((int64)total_sz * widthScale) >= INT_MAX;
        if (isContiguous && !has_int_overflow)
            total = 1;                            // vector-row
        m1 = m1.reshape(0, total);
        m2 = m2.reshape(0, total);
        HL_Assert(m1.cols == m2.cols && m1.rows == m2.rows);
        return Size(m1.cols * widthScale, m1.rows);
    }
    return getContinuousSize_(m1.flags & m2.flags, m1.cols, m1.rows, widthScale);
}

Size getContinuousSize2D(Mat& m1, Mat& m2, Mat& m3, int widthScale)
{
    HL_CheckLE(m1.dims, 2, "");
    HL_CheckLE(m2.dims, 2, "");
    HL_CheckLE(m3.dims, 2, "");
    const Size sz1 = m1.size();
    if (sz1 != m2.size() || sz1 != m3.size())    // reshape all matrixes to the same size (#4159)
    {
        size_t total_sz = m1.total();
        HL_CheckEQ(total_sz, m2.total(), "");
        HL_CheckEQ(total_sz, m3.total(), "");
        bool is_m1_vector = m1.cols == 1 || m1.rows == 1;
        bool is_m2_vector = m2.cols == 1 || m2.rows == 1;
        bool is_m3_vector = m3.cols == 1 || m3.rows == 1;
        HL_Assert(is_m1_vector);
        HL_Assert(is_m2_vector);
        HL_Assert(is_m3_vector);
        int  total            = (int)total_sz;    // vector-column
        bool isContiguous     = ((m1.flags & m2.flags & m3.flags) & Mat::CONTINUOUS_FLAG) != 0;
        bool has_int_overflow = ((int64)total_sz * widthScale) >= INT_MAX;
        if (isContiguous && !has_int_overflow)
            total = 1;                            // vector-row
        m1 = m1.reshape(0, total);
        m2 = m2.reshape(0, total);
        m3 = m3.reshape(0, total);
        HL_Assert(m1.cols == m2.cols && m1.rows == m2.rows && m1.cols == m3.cols && m1.rows == m3.rows);
        return Size(m1.cols * widthScale, m1.rows);
    }
    return getContinuousSize_(m1.flags & m2.flags & m3.flags, m1.cols, m1.rows, widthScale);
}


}    // namespace hl
