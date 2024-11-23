#include "precomp.h"
#include "color.h"
#include "color_rgb.simd.h"

void cvtBGRtoGray(const unsigned char* src_data, size_t src_step, unsigned char* dst_data, size_t dst_step, int width, int height, int depth, int scn)
{
    cvtBGRtoGray_cpu_baseline(src_data, src_step, dst_data, dst_step, width, height, depth, scn);
}

void cvtColorBGR2Gray(const Mat* src, Mat* dst)
{
    CvtHelper h;
    initCvtHelper(&h, src, dst, 1, NONE);
    cvtBGRtoGray(h.src.data, h.src.step[0], h.dst.data, h.dst.step[0], h.src.cols, h.src.rows, h.depth, h.scn);
}