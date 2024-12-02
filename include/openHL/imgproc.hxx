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

enum FlipType {
    HORIZONTAL,
    VERTICAL
};

const double PI = 3.14159265358979323846;

void cvtColor(const Mat& src, Mat& dst, int code, int dstCn = 0);
void translate(const Mat& src, Mat& dst, int tx, int ty);
void resize(const Mat& src, Mat& dst, int newWidth, int newHeight);
void flip(const Mat& src, Mat& dst, FlipType flipType);
void rotate(const Mat& src, Mat& dst, double angle);

}    // namespace hl