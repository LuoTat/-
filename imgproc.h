#pragma once

#include "core.h"

enum ColorConversionCodes
{
    COLOR_BGR2GRAY,
    COLOR_BGR2GRAY_INVERTED
};

// 颜色空间转换
void cvtColor(const Mat* src, Mat* dst, enum ColorConversionCodes code);
