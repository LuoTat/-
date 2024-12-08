#pragma once

#include "openHL/core.hxx"

namespace hl
{

enum ThresholdTypes
{
    THRESH_BINARY     = 0,
    THRESH_BINARY_INV = 1,
    THRESH_TRUNC      = 2,
    THRESH_TOZERO     = 3,
    THRESH_TOZERO_INV = 4,
    THRESH_MASK       = 7,
    THRESH_OTSU       = 8,
    THRESH_TRIANGLE   = 16
};

enum ColorConversionCodes
{
    COLOR_BGR2GRAY    = 6,
    COLOR_RGB2GRAY    = 7,
    COLOR_BGRA2GRAY   = 10,
    COLOR_RGBA2GRAY   = 11,
    COLOR_BGR5652GRAY = 21,
    COLOR_BGR5552GRAY = 31,
};

void medianBlur(const Mat& src, Mat& dst, int ksize);

void boxFilter(const Mat& src, Mat& dst, int ddepth, Size ksize, Point anchor = Point(-1, -1), bool normalize = true, int borderType = BORDER_DEFAULT);

void blur(const Mat& src, Mat& dst, Size ksize, Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

double threshold(const Mat& src, Mat& dst, double thresh, double maxval, int type);

void calcHist(const Mat* images, int nimages, const int* channels, const Mat& mask, Mat& hist, int dims, const int* histSize, const float** ranges, bool uniform = true, bool accumulate = false);

void equalizeHist(const Mat& src, Mat& dst);

void drawHist(const Mat& hist, Mat& histImage, uint width, uint height);

void cvtColor(const Mat& src, Mat& dst, int code, int dstCn = 0);

}    // namespace hl