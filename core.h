#pragma once

#include "base.h"
#include "mat.h"

// 分离通道
void split(const Mat* src, Mat* mvbegin);

// 归一化
void normalize(const Mat* src, Mat* dst, int min, int max, enum NormTypes norm_type);

// 获取最大最小值
// src只支持单通道和size_t类型
void minMaxIdx(const Mat* src, double* minVal, double* maxVal, int* minIdx, int* maxIdx);