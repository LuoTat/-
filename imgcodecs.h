#pragma once

#include "core.h"

// 从文件中读取bmp图像，返回一个Mat
Mat imread(const char* filename);

// 将Mat写入bmp文件
void imwrite(const char* filename, const Mat* img);