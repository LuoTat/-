#pragma once

#include "openHL/core.hxx"

namespace hl
{

enum ColorConversionCodes
{
    COLOR_BGR2GRAY    = 6,
    COLOR_RGB2GRAY    = 7,
    COLOR_BGRA2GRAY   = 10,
    COLOR_RGBA2GRAY   = 11,
    COLOR_BGR5652GRAY = 21,
    COLOR_BGR5552GRAY = 31,
};

void calcHist(const Mat* images, int nimages, const int* channels, const Mat& mask, Mat& hist, int dims, const int* histSize, const float** ranges, bool uniform = true, bool accumulate = false);

void cvtColor(const Mat& src, Mat& dst, int code, int dstCn = 0);

}    // namespace hl