#pragma once

#include "openHL/core.hxx"

namespace hl
{

enum ImreadModes
{
    IMREAD_UNCHANGED           = -1,
    IMREAD_GRAYSCALE           = 0,
    IMREAD_COLOR               = 1,
    IMREAD_ANYDEPTH            = 2,
    IMREAD_ANYCOLOR            = 4,
    IMREAD_LOAD_GDAL           = 8,
    IMREAD_REDUCED_GRAYSCALE_2 = 16,
    IMREAD_REDUCED_COLOR_2     = 17,
    IMREAD_REDUCED_GRAYSCALE_4 = 32,
    IMREAD_REDUCED_COLOR_4     = 33,
    IMREAD_REDUCED_GRAYSCALE_8 = 64,
    IMREAD_REDUCED_COLOR_8     = 65,
    IMREAD_IGNORE_ORIENTATION  = 128
};

Mat imread(const String& filename, int flags = IMREAD_COLOR);

bool imwrite(const String& filename, const Mat& img, const std::vector<int>& params = std::vector<int>());
}    // namespace hl