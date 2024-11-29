#pragma once

#include "openHL/imgproc.hxx"

namespace hl
{
namespace impl
{

#include "color.simd_helpers.hxx"

inline bool swapBlue(int code)
{
    switch (code)
    {
        case COLOR_BGR2GRAY :
        case COLOR_BGRA2GRAY :
            return false;
        default :
            return true;
    }
}

inline int dstChannels(int code)
{
    switch (code)
    {
        // return 4;
        // return 3;
        // return 2;
        default :
            return 0;
    }
}
}    // namespace impl

using namespace impl;

void cvtColorBGR2Gray(const Mat& _src, Mat& _dst, bool swapb);
}    // namespace hl