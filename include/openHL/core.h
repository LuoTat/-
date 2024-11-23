#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/base.h"
#include "openHL/core/hlstd.h"
#include "openHL/core/mat.h"

// 分离通道
void split(const Mat* src, Mat* mvbegin);

// 归一化
void normalize(const Mat* src, Mat* dst, int min, int max, NormTypes norm_type);

// 获取最大最小值
void minMaxIdx(const Mat* src, double* minVal, double* maxVal, int* minIdx, int* maxIdx);