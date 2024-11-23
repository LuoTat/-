#pragma once

#include "openHL/core.h"

#include <float.h>
#include <math.h>


typedef void (*BinaryFunc)(const unsigned char* src1, size_t step1, const unsigned char* src2, size_t step2, unsigned char* dst, size_t step, int width, int height, void*);

BinaryFunc getConvertFunc(int sdepth, int ddepth);
BinaryFunc getConvertScaleFunc(int sdepth, int ddepth);