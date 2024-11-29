#define HL_DESCALE(x, n) (((x) + (1 << ((n) - 1))) >> (n))

namespace
{

//constants for conversion from/to RGB and Gray, YUV, YCrCb according to BT.601
const static float B2YF = 0.114f;
const static float G2YF = 0.587f;
const static float R2YF = 0.299f;

enum
{
    gray_shift = 15,
    yuv_shift  = 14,
    xyz_shift  = 12,
    R2Y        = 4899,     // == R2YF*16384
    G2Y        = 9617,     // == G2YF*16384
    B2Y        = 1868,     // == B2YF*16384
    RY15       = 9798,     // == R2YF*32768 + 0.5
    GY15       = 19235,    // == G2YF*32768 + 0.5
    BY15       = 3735,     // == B2YF*32768 + 0.5
    BLOCK_SIZE = 256
};

template <typename _Tp> struct ColorChannel
{
    typedef float worktype_f;

    inline static _Tp max() { return std::numeric_limits<_Tp>::max(); }

    inline static _Tp half() { return (_Tp)(max() / 2 + 1); }
};

template <> struct ColorChannel<float>
{
    typedef float worktype_f;

    inline static float max() { return 1.f; }

    inline static float half() { return 0.5f; }
};

template <int i0, int i1 = -1, int i2 = -1>
struct Set
{
    inline static bool contains(int i)
    {
        return (i == i0 || i == i1 || i == i2);
    }
};

template <int i0, int i1>
struct Set<i0, i1, -1>
{
    inline static bool contains(int i)
    {
        return (i == i0 || i == i1);
    }
};

template <int i0>
struct Set<i0, -1, -1>
{
    inline static bool contains(int i)
    {
        return (i == i0);
    }
};

enum SizePolicy
{
    TO_YUV,
    FROM_YUV,
    FROM_UYVY,
    TO_UYVY,
    NONE
};

template <typename VScn, typename VDcn, typename VDepth, SizePolicy sizePolicy = NONE>
struct CvtHelper
{
    CvtHelper(const Mat& _src, Mat& _dst, int dcn)
    {
        HL_Assert(!_src.empty());

        int stype = _src.type();
        scn = HL_MAT_CN(stype), depth = HL_MAT_DEPTH(stype);

        HL_Check(VScn::contains(scn), "Invalid number of channels in input image");
        HL_Check(VDcn::contains(dcn), "Invalid number of channels in output image");
        HL_CheckDepth(VDepth::contains(depth), "Unsupported depth of input image");

        if (&_src == &_dst)    // inplace processing (#6653)
            _src.copyTo(src);
        else
            src = _src;
        Size sz = src.size();
        switch (sizePolicy)
        {
            case TO_YUV :
                HL_Assert(sz.width % 2 == 0 && sz.height % 2 == 0);
                dstSz = Size(sz.width, sz.height / 2 * 3);
                break;
            case FROM_YUV :
                HL_Assert(sz.width % 2 == 0 && sz.height % 3 == 0);
                dstSz = Size(sz.width, sz.height * 2 / 3);
                break;
            case FROM_UYVY :
            case TO_UYVY :
                HL_Assert(sz.width % 2 == 0);
                dstSz = sz;
                break;
            case NONE :
            default :
                dstSz = sz;
                break;
        }
        _dst.create(dstSz, HL_MAKETYPE(depth, dcn));
        dst = _dst;
    }

    Mat  src, dst;
    int  depth, scn;
    Size dstSz;
};

///////////////////////////// Top-level template function ////////////////////////////////

template <typename Cvt>
class CvtColorLoop_Invoker: public ParallelLoopBody
{
    typedef typename Cvt::channel_type _Tp;

public:
    CvtColorLoop_Invoker(const uchar* src_data_, size_t src_step_, uchar* dst_data_, size_t dst_step_, int width_, const Cvt& _cvt):
        ParallelLoopBody(), src_data(src_data_), src_step(src_step_), dst_data(dst_data_), dst_step(dst_step_), width(width_), cvt(_cvt)
    {
    }

    virtual void operator()(const Range& range) const override
    {
        const uchar* yS = src_data + static_cast<size_t>(range.start) * src_step;
        uchar*       yD = dst_data + static_cast<size_t>(range.start) * dst_step;

        for (int i = range.start; i < range.end; ++i, yS += src_step, yD += dst_step)
            cvt(reinterpret_cast<const _Tp*>(yS), reinterpret_cast<_Tp*>(yD), width);
    }

private:
    const uchar* src_data;
    const size_t src_step;
    uchar*       dst_data;
    const size_t dst_step;
    const int    width;
    const Cvt&   cvt;

    CvtColorLoop_Invoker(const CvtColorLoop_Invoker&);                     // = delete;
    const CvtColorLoop_Invoker& operator=(const CvtColorLoop_Invoker&);    // = delete;
};

template <typename Cvt>
inline static void CvtColorLoop(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, const Cvt& cvt)
{
    parallel_for_(Range(0, height),
                  CvtColorLoop_Invoker<Cvt>(src_data, src_step, dst_data, dst_step, width, cvt),
                  (width * height) / static_cast<double>(1 << 16));
}

}    //namespace
