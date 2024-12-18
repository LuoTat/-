#include "precomp.hxx"
#include "color.hxx"
#include "color_rgb.simd.hxx"

namespace hl
{
namespace hal
{

void cvtBGRtoGray(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int scn, bool swapBlue)
{
    cpu_baseline::cvtBGRtoGray(src_data, src_step, dst_data, dst_step, width, height, depth, scn, swapBlue);
}

// 8u, 16u, 32f
void cvtGraytoBGR(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int dcn)
{
    cpu_baseline::cvtGraytoBGR(src_data, src_step, dst_data, dst_step, width, height, depth, dcn);
}

}    // namespace hal

void cvtColorBGR2Gray(const Mat& _src, Mat& _dst, bool swapb)
{
    CvtHelper<Set<3, 4>, Set<1>, Set<HL_8U, HL_16U, HL_32F>> h(_src, _dst, 1);

    hal::cvtBGRtoGray(h.src.data, h.src.step, h.dst.data, h.dst.step, h.src.cols, h.src.rows, h.depth, h.scn, swapb);
}

void cvtColorGray2BGR(const Mat& _src, Mat& _dst, int dcn)
{
    if (dcn <= 0) dcn = 3;
    CvtHelper<Set<1>, Set<3, 4>, Set<HL_8U, HL_16U, HL_32F>> h(_src, _dst, dcn);

    hal::cvtGraytoBGR(h.src.data, h.src.step, h.dst.data, h.dst.step, h.src.cols, h.src.rows, h.depth, dcn);
}

}    // namespace hl