#pragma once

#include "mat.h"

// 从文件中读取bmp图像，返回一个Mat
Mat imread(const char* filename);

// 将bmp图像写入文件
void imwrite(const char* filename, Mat* img);