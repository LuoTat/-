#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.hxx"
#include "openHL/core/hal/interface.h"

namespace hl
{
namespace hal
{

void resize(int src_type, const uchar* src_data, size_t src_step, int src_width, int src_height, uchar* dst_data, size_t dst_step, int dst_width, int dst_height, double inv_scale_x, double inv_scale_y, int interpolation);

void warpAffine(int src_type, const uchar* src_data, size_t src_step, int src_width, int src_height, uchar* dst_data, size_t dst_step, int dst_width, int dst_height, const double M[6], int interpolation, int borderType, const double borderValue[4]);

void warpAffineBlocklineNN(int* adelta, int* bdelta, short* xy, int X0, int Y0, int bw);

void warpAffineBlockline(int* adelta, int* bdelta, short* xy, short* alpha, int X0, int Y0, int bw);

void cvtBGRtoGray(const uchar* src_data, size_t src_step, uchar* dst_data, size_t dst_step, int width, int height, int depth, int scn, bool swapBlue);

}    // namespace hal
}    // namespace hl