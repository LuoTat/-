#pragma once

#include "openHL/core.h"

enum ImreadModes
{
    IMREAD_UNCHANGED = -1,
    IMREAD_GRAYSCALE = 0,
    IMREAD_COLOR     = 1,
    IMREAD_ANYDEPTH  = 2,
    IMREAD_ANYCOLOR  = 4,
};

// 从文件中读取bmp图像，返回一个Mat
Mat imread(const char* filename, int flags);

// 将Mat写入bmp文件
bool imwrite(const char* filename, const Mat* img);