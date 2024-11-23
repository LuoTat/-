#pragma once

#include "openHL/core.h"

enum ColorConversionCodes
{
    COLOR_BGR2GRAY = 6
};

// 颜色空间转换
void cvtColor(const Mat* src, Mat* dst, int code);

// 直方图计算
void calcHist(const Mat* images, Mat* hist);

// 直方图均衡化
void equalizeImage(Mat* image);

// 画直方图
void drawHist(const Mat* hist, Mat* histImage, int width, int height);
