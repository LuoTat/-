#pragma once

#include "openHL/core.hxx"

namespace hl
{

enum InterpolationFlags
{
    INTER_NEAREST       = 0,
    INTER_LINEAR        = 1,
    INTER_CUBIC         = 2,
    INTER_AREA          = 3,
    INTER_LANCZOS4      = 4,
    INTER_LINEAR_EXACT  = 5,
    INTER_NEAREST_EXACT = 6,
    INTER_MAX           = 7,
    WARP_FILL_OUTLIERS  = 8,
    WARP_INVERSE_MAP    = 16,
    WARP_RELATIVE_MAP   = 32
};

enum InterpolationMasks
{
    INTER_BITS      = 5,
    INTER_BITS2     = INTER_BITS * 2,
    INTER_TAB_SIZE  = 1 << INTER_BITS,
    INTER_TAB_SIZE2 = INTER_TAB_SIZE * INTER_TAB_SIZE
};

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

void prewitt(const Mat& src, Mat& dst);

void sobel(const Mat& src, Mat& dst);

void LOG(const Mat& src, Mat& dst);

double threshold_Iter(const Mat& src, Mat& dst, const Mat& hist, double epsilon);

void regionSplitting(const Mat& src, Mat& dst, int threshold);

void regionGrowing(const Mat& src, Mat& dst, int seedX, int seedY, int threshold);

void medianBlur(const Mat& src, Mat& dst, int ksize);

void boxFilter(const Mat& src, Mat& dst, int ddepth, Size ksize, Point anchor = Point(-1, -1), bool normalize = true, int borderType = BORDER_DEFAULT);

void blur(const Mat& src, Mat& dst, Size ksize, Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT);

void resize(const Mat& src, Mat& dst, Size dsize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR);

void warpAffine(const Mat& src, Mat& dst, const Mat& M, Size dsize, int flags = INTER_LINEAR, int borderMode = BORDER_CONSTANT, const Scalar& borderValue = Scalar());

void remap(const Mat& src, Mat& dst, const Mat& map1, const Mat& map2, int interpolation, int borderMode = BORDER_CONSTANT, const Scalar& borderValue = Scalar());

Matx23d getRotationMatrix2D_(Point2f center, double angle, double scale);

inline Mat getRotationMatrix2D(Point2f center, double angle, double scale)
{
    return Mat(getRotationMatrix2D_(center, angle, scale), true);
}

double threshold(const Mat& src, Mat& dst, double thresh, double maxval, int type);

void calcHist(const Mat* images, int nimages, const int* channels, const Mat& mask, Mat& hist, int dims, const int* histSize, const float** ranges, bool uniform = true, bool accumulate = false);

void equalizeHist(const Mat& src, Mat& dst);

void drawHist_T(const Mat& hist, Mat& histImage, uint width, uint height, uchar thresh);

void drawHist(const Mat& hist, Mat& histImage, uint width, uint height);

void cvtColor(const Mat& src, Mat& dst, int code, int dstCn = 0);

}    // namespace hl