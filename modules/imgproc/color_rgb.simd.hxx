#include "precomp.hxx"

namespace hl
{
namespace hal
{
namespace cpu_baseline
{


void cvtBGRtoGray(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int scn, bool swapBlue);

void cvtGraytoBGR(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int dcn);

namespace
{

///////////////////////////////// Color to/from Grayscale ////////////////////////////////

template <typename _Tp>
struct Gray2RGB
{
    typedef _Tp channel_type;

    Gray2RGB(int _dstcn):
        dstcn(_dstcn) {}

    void operator()(const _Tp* src, _Tp* dst, int n) const
    {
        int dcn   = dstcn;
        int i     = 0;
        _Tp alpha = ColorChannel<_Tp>::max();

        for (; i < n; i++, src++, dst += dcn)
        {
            dst[0] = dst[1] = dst[2] = src[0];
            if (dcn == 4)
                dst[3] = alpha;
        }
    }

    int dstcn;
};

template <typename _Tp> struct RGB2Gray
{
    typedef _Tp channel_type;

    RGB2Gray(int _srccn, int blueIdx, const float* _coeffs):
        srccn(_srccn)
    {
        const static float coeffs0[] = {R2YF, G2YF, B2YF};
        memcpy(coeffs, _coeffs ? _coeffs : coeffs0, 3 * sizeof(coeffs[0]));
        if (blueIdx == 0)
            std::swap(coeffs[0], coeffs[2]);
    }

    void operator()(const _Tp* src, _Tp* dst, int n) const
    {
        int   scn = srccn;
        float cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];
        for (int i = 0; i < n; i++, src += scn)
            dst[i] = saturate_cast<_Tp>(src[0] * cb + src[1] * cg + src[2] * cr);
    }

    int   srccn;
    float coeffs[3];
};

template <>
struct RGB2Gray<uchar>
{
    typedef uchar channel_type;

    const static int BY    = BY15;
    const static int GY    = GY15;
    const static int RY    = RY15;
    const static int shift = gray_shift;

    RGB2Gray(int _srccn, int blueIdx, const int* _coeffs):
        srccn(_srccn)
    {
        const int coeffs0[] = {RY, GY, BY};
        for (int i = 0; i < 3; i++)
            coeffs[i] = (short)(_coeffs ? _coeffs[i] : coeffs0[i]);
        if (blueIdx == 0)
            std::swap(coeffs[0], coeffs[2]);

        HL_Assert(coeffs[0] + coeffs[1] + coeffs[2] == (1 << shift));
    }

    void operator()(const uchar* src, uchar* dst, int n) const
    {
        int   scn = srccn;
        short cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];
        int   i = 0;

        for (; i < n; i++, src += scn, dst++)
        {
            int   b = src[0], g = src[1], r = src[2];
            uchar y = (uchar)HL_DESCALE(b * cb + g * cg + r * cr, shift);
            dst[0]  = y;
        }
    }

    int   srccn;
    short coeffs[3];
};

template <>
struct RGB2Gray<ushort>
{
    typedef ushort channel_type;

    const static int BY        = BY15;
    const static int GY        = GY15;
    const static int RY        = RY15;
    const static int shift     = gray_shift;
    const static int fix_shift = (int)(sizeof(short) * 8 - shift);

    RGB2Gray(int _srccn, int blueIdx, const int* _coeffs):
        srccn(_srccn)
    {
        const int coeffs0[] = {RY, GY, BY};
        for (int i = 0; i < 3; i++)
            coeffs[i] = (short)(_coeffs ? _coeffs[i] : coeffs0[i]);
        if (blueIdx == 0)
            std::swap(coeffs[0], coeffs[2]);

        HL_Assert(coeffs[0] + coeffs[1] + coeffs[2] == (1 << shift));
    }

    void operator()(const ushort* src, ushort* dst, int n) const
    {
        int   scn = srccn;
        short cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];
        int   i = 0;

        for (; i < n; i++, src += scn, dst++)
        {
            int    b = src[0], g = src[1], r = src[2];
            ushort d = (ushort)HL_DESCALE((unsigned)(b * cb + g * cg + r * cr), shift);
            dst[0]   = d;
        }
    }

    int   srccn;
    short coeffs[3];
};

template <>
struct RGB2Gray<float>
{
    typedef float channel_type;

    RGB2Gray(int _srccn, int blueIdx, const float* _coeffs):
        srccn(_srccn)
    {
        const static float coeffs0[] = {R2YF, G2YF, B2YF};
        for (int i = 0; i < 3; i++)
        {
            coeffs[i] = _coeffs ? _coeffs[i] : coeffs0[i];
        }
        if (blueIdx == 0)
            std::swap(coeffs[0], coeffs[2]);
    }

    void operator()(const float* src, float* dst, int n) const
    {
        int   scn = srccn, i = 0;
        float cb = coeffs[0], cg = coeffs[1], cr = coeffs[2];

        for (; i < n; i++, src += scn, dst++)
            dst[0] = src[0] * cb + src[1] * cg + src[2] * cr;
    }

    int   srccn;
    float coeffs[3];
};

}    // namespace

void cvtBGRtoGray(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int scn, bool swapBlue)
{
    int blueIdx = swapBlue ? 2 : 0;
    if (depth == HL_8U)
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<uchar>(scn, blueIdx, 0));
    else if (depth == HL_16U)
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<ushort>(scn, blueIdx, 0));
    else
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray<float>(scn, blueIdx, 0));
}

// 8u, 16u, 32f
void cvtGraytoBGR(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int dcn)
{
    if (depth == HL_8U)
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, Gray2RGB<uchar>(dcn));
    else if (depth == HL_16U)
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, Gray2RGB<ushort>(dcn));
    else
        CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, Gray2RGB<float>(dcn));
}



}    // namespace cpu_baseline
}    // namespace hal
}    // namespace hl