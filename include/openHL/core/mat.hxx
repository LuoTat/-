#pragma once

#include "openHL/core/matx.hxx"
#include "openHL/core/types.hxx"
#include "openHL/core/bufferpool.hxx"

namespace hl
{

enum AccessFlag
{
    ACCESS_READ  = 1 << 24,
    ACCESS_WRITE = 1 << 25,
    ACCESS_RW    = 3 << 24,
    ACCESS_MASK  = ACCESS_RW,
    ACCESS_FAST  = 1 << 26
};

enum UMatUsageFlags
{
    USAGE_DEFAULT                = 0,
    USAGE_ALLOCATE_HOST_MEMORY   = 1 << 0,
    USAGE_ALLOCATE_DEVICE_MEMORY = 1 << 1,
    USAGE_ALLOCATE_SHARED_MEMORY = 1 << 2,
    __UMAT_USAGE_FLAGS_32BIT     = 0x7fffffff
};

struct UMatData;

class MatAllocator
{
public:
    MatAllocator() {}

    virtual ~MatAllocator() {}

    virtual UMatData* allocate(int dims, const int* sizes, int type, void* data, size_t* step, AccessFlag flags, UMatUsageFlags usageFlags) const = 0;
    virtual bool      allocate(UMatData* data, AccessFlag accessflags, UMatUsageFlags usageFlags) const                                           = 0;
    virtual void      deallocate(UMatData* data) const                                                                                            = 0;
    virtual void      map(UMatData* data, AccessFlag accessflags) const;
    virtual void      unmap(UMatData* data) const;
    virtual void      download(UMatData* data, void* dst, int dims, const size_t sz[], const size_t srcofs[], const size_t srcstep[], const size_t dststep[]) const;
    virtual void      upload(UMatData* data, const void* src, int dims, const size_t sz[], const size_t dstofs[], const size_t dststep[], const size_t srcstep[]) const;
    virtual void      copy(UMatData* srcdata, UMatData* dstdata, int dims, const size_t sz[], const size_t srcofs[], const size_t srcstep[], const size_t dstofs[], const size_t dststep[], bool sync) const;

    virtual BufferPoolController* getBufferPoolController(const char* id = NULL) const;
};

struct UMatData
{
    enum MemoryFlag
    {
        COPY_ON_MAP          = 1,
        HOST_COPY_OBSOLETE   = 2,
        DEVICE_COPY_OBSOLETE = 4,
        TEMP_UMAT            = 8,
        TEMP_COPIED_UMAT     = 24,
        USER_ALLOCATED       = 32,
        DEVICE_MEM_MAPPED    = 64,
        ASYNC_CLEANUP        = 128
    };

    UMatData(const MatAllocator* allocator);
    ~UMatData();

    void lock();
    void unlock();

    bool hostCopyObsolete() const;
    bool deviceCopyObsolete() const;
    bool deviceMemMapped() const;
    bool copyOnMap() const;
    bool tempUMat() const;
    bool tempCopiedUMat() const;
    void markHostCopyObsolete(bool flag);
    void markDeviceCopyObsolete(bool flag);
    void markDeviceMemMapped(bool flag);

    const MatAllocator* prevAllocator;
    const MatAllocator* currAllocator;
    int                 urefcount;
    int                 refcount;
    uchar*              data;
    uchar*              origdata;
    size_t              size;

    UMatData::MemoryFlag  flags;
    void*                 handle;
    void*                 userdata;
    int                   allocatorFlags_;
    int                   mapcount;
    UMatData*             originalUMatData;
    std::shared_ptr<void> allocatorContext;
};
HL_ENUM_FLAGS(UMatData::MemoryFlag)

struct MatSize
{
    explicit MatSize(int* _p) noexcept;
    int        dims() const noexcept;
    Size       operator()() const;
    const int& operator[](int i) const;
    int&       operator[](int i);
    operator const int*() const noexcept;
    bool operator==(const MatSize& sz) const noexcept;
    bool operator!=(const MatSize& sz) const noexcept;

    int* p;
};

struct MatStep
{
    MatStep() noexcept;
    explicit MatStep(size_t s) noexcept;
    const size_t& operator[](int i) const noexcept;
    size_t&       operator[](int i) noexcept;
    operator size_t() const;
    MatStep& operator=(size_t s);

    size_t* p;
    size_t  buf[2];

protected:
    MatStep& operator=(const MatStep&);
};

class Mat
{
public:
    Mat() noexcept;
    Mat(const Mat& m);
    Mat(Mat&& m);

    Mat(const double& val);

    Mat(int rows, int cols, int type);
    Mat(int rows, int cols, int type, const Scalar& s);
    Mat(int rows, int cols, int type, void* data, size_t step = AUTO_STEP);

    Mat(Size size, int type);
    Mat(Size size, int type, const Scalar& s);
    Mat(Size size, int type, void* data, size_t step = AUTO_STEP);

    Mat(int ndims, const int* sizes, int type);
    Mat(int ndims, const int* sizes, int type, const Scalar& s);
    Mat(int ndims, const int* sizes, int type, void* data, const size_t* steps = 0);

    Mat(const std::vector<int>& sizes, int type);
    Mat(const std::vector<int>& sizes, int type, const Scalar& s);
    Mat(const std::vector<int>& sizes, int type, void* data, const size_t* steps = 0);

    Mat(const Mat& m, const Range& rowRange, const Range& colRange = Range::all());
    Mat(const Mat& m, const Rect& roi);
    Mat(const Mat& m, const Range* ranges);
    Mat(const Mat& m, const std::vector<Range>& ranges);

    template <typename _Tp>
    explicit Mat(const std::vector<_Tp>& vec, bool copyData = false);

    template <typename _Tp, typename = typename std::enable_if<std::is_arithmetic<_Tp>::value>::type>
    explicit Mat(const std::initializer_list<_Tp> list);

    template <typename _Tp>
    explicit Mat(const std::initializer_list<int> sizes, const std::initializer_list<_Tp> list);

    template <typename _Tp, size_t _Nm>
    explicit Mat(const std::array<_Tp, _Nm>& arr, bool copyData = false);

    template <typename _Tp, int n>
    explicit Mat(const Vec<_Tp, n>& vec, bool copyData = true);

    template <typename _Tp, int m, int n>
    explicit Mat(const Matx<_Tp, m, n>& mtx, bool copyData = true);

    template <typename _Tp>
    explicit Mat(const Point_<_Tp>& pt, bool copyData = true);

    template <typename _Tp>
    explicit Mat(const Point3_<_Tp>& pt, bool copyData = true);


    ~Mat();

    Mat& operator=(const Mat& m);
    Mat& operator=(Mat&& m);
    Mat& operator=(const Scalar& s);
    Mat  operator()(Range rowRange, Range colRange) const;
    Mat  operator()(const Rect& roi) const;
    Mat  operator()(const Range* ranges) const;
    Mat  operator()(const std::vector<Range>& ranges) const;


    Mat row(int y) const;
    Mat col(int x) const;
    Mat rowRange(int startrow, int endrow) const;
    Mat rowRange(const Range& r) const;
    Mat colRange(int startcol, int endcol) const;
    Mat colRange(const Range& r) const;

    [[nodiscard]]
    Mat  clone() const;
    void copyTo(Mat& m) const;
    void copyTo(Mat& m, const Mat& mask) const;
    void convertTo(Mat& m, int rtype, double alpha = 1, double beta = 0) const;
    Mat& setTo(const Mat& value, const Mat& mask = Mat());

    Mat reshape(int cn, int rows = 0) const;
    Mat reshape(int cn, int newndims, const int* newsz) const;

    void create(int rows, int cols, int type);
    void create(Size size, int type);
    void create(int ndims, const int* sizes, int type);
    void create(const std::vector<int>& sizes, int type);

    void release();
    void locateROI(Size& wholeSize, Point& ofs) const;
    Mat& adjustROI(int dtop, int dbottom, int dleft, int dright);

    bool   isContinuous() const;
    bool   isSubmatrix() const;
    size_t elemSize() const;
    size_t elemSize1() const;
    int    type() const;
    int    depth() const;
    int    channels() const;
    size_t step1(int i = 0) const;
    bool   empty() const;
    size_t total() const;
    size_t total(int startDim, int endDim = INT_MAX) const;

    uchar*                             ptr(int i0 = 0);
    const uchar*                       ptr(int i0 = 0) const;
    template <typename _Tp> _Tp*       ptr(int i0 = 0);
    template <typename _Tp> const _Tp* ptr(int i0 = 0) const;

    uchar*                             ptr(int row, int col);
    const uchar*                       ptr(int row, int col) const;
    template <typename _Tp> _Tp*       ptr(int row, int col);
    template <typename _Tp> const _Tp* ptr(int row, int col) const;

    uchar*                             ptr(int i0, int i1, int i2);
    const uchar*                       ptr(int i0, int i1, int i2) const;
    template <typename _Tp> _Tp*       ptr(int i0, int i1, int i2);
    template <typename _Tp> const _Tp* ptr(int i0, int i1, int i2) const;

    uchar*                             ptr(const int* idx);
    const uchar*                       ptr(const int* idx) const;
    template <typename _Tp> _Tp*       ptr(const int* idx);
    template <typename _Tp> const _Tp* ptr(const int* idx) const;

    template <typename _Tp> _Tp&       at(int i0 = 0);
    template <typename _Tp> const _Tp& at(int i0 = 0) const;

    template <typename _Tp> _Tp&       at(int row, int col);
    template <typename _Tp> const _Tp& at(int row, int col) const;

    template <typename _Tp> _Tp&       at(int i0, int i1, int i2);
    template <typename _Tp> const _Tp& at(int i0, int i1, int i2) const;

    template <typename _Tp> _Tp&       at(const int* idx);
    template <typename _Tp> const _Tp& at(const int* idx) const;

    enum
    {
        MAGIC_VAL       = 0x42FF0000,
        AUTO_STEP       = 0,
        CONTINUOUS_FLAG = HL_MAT_CONT_FLAG,
        SUBMATRIX_FLAG  = HL_SUBMAT_FLAG
    };

    enum
    {
        MAGIC_MASK = 0xFFFF0000,
        TYPE_MASK  = 0x00000FFF,
        DEPTH_MASK = 7
    };

    void                 addref();
    void                 deallocate();
    void                 copySize(const Mat& m);
    void                 updateContinuityFlag();
    static MatAllocator* getStdAllocator();
    static MatAllocator* getDefaultAllocator();
    static void          setDefaultAllocator(MatAllocator* allocator);

    int    flags;
    int    dims;
    int    rows, cols;
    uchar* data;

    const uchar* datastart;
    const uchar* dataend;
    const uchar* datalimit;

    MatAllocator* allocator;
    UMatData*     u;
    MatSize       size;
    MatStep       step;
};

class NAryMatIterator
{
public:
    NAryMatIterator();
    NAryMatIterator(const Mat** arrays, uchar** ptrs, int narrays = -1);
    NAryMatIterator(const Mat** arrays, Mat* planes, int narrays = -1);

    NAryMatIterator& operator++();
    NAryMatIterator  operator++(int);

    void init(const Mat** arrays, Mat* planes, uchar** ptrs, int narrays = -1);

    const Mat** arrays;
    Mat*        planes;
    uchar**     ptrs;
    int         narrays;
    size_t      nplanes;
    size_t      size;

protected:
    int    iterdepth;
    size_t idx;
};

}    // namespace hl

#include "openHL/core/mat.inl.hxx"