#pragma once

#include "openHL/core/utility.hxx"
#include "openHL/core/core_c.h"
#include "openHL/core/private.hxx"

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <float.h>
#include <cstring>
#include <cassert>

#include "openHL/core/hal/hal.hxx"
#include "openHL/core/hal/intrin.hxx"

namespace hl
{

typedef void (*BinaryFunc)(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, Size sz, void*);

typedef void (*BinaryFuncC)(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);

BinaryFunc getConvertFunc(int sdepth, int ddepth);
BinaryFunc getConvertScaleFunc(int sdepth, int ddepth);
BinaryFunc getCopyMaskFunc(size_t esz);

Size getContinuousSize2D(Mat& m1, int widthScale = 1);
Size getContinuousSize2D(Mat& m1, Mat& m2, int widthScale = 1);
Size getContinuousSize2D(Mat& m1, Mat& m2, Mat& m3, int widthScale = 1);

void setSize(Mat& m, int _dims, const int* _sz, const size_t* _steps, bool autoSteps = false);
void finalizeHdr(Mat& m);
int  updateContinuityFlag(int flags, int dims, const int* size, const size_t* step);

#define HL_SPLIT_MERGE_MAX_BLOCK_SIZE(cn) ((INT_MAX / 4) / (cn))

enum
{
    BLOCK_SIZE = 1024
};

inline bool checkScalar(const Mat& sc, int atype)
{
    if (sc.dims > 2 || !sc.isContinuous())
        return false;
    Size sz = sc.size();
    if (sz.width != 1 && sz.height != 1)
        return false;
    int cn = HL_MAT_CN(atype);

    return sz == Size(1, 1) || sz == Size(1, cn) || sz == Size(cn, 1) || (sz == Size(1, 4) && sc.type() == HL_64F && cn <= 4);
}

void convertAndUnrollScalar(const Mat& sc, int buftype, uchar* scbuf, size_t blocksize);


#define HL_SINGLETON_LAZY_INIT_(TYPE, INITIALIZER, RET_VALUE) \
    static TYPE* const instance = INITIALIZER;                \
    return RET_VALUE;

#define HL_SINGLETON_LAZY_INIT(TYPE, INITIALIZER)     HL_SINGLETON_LAZY_INIT_(TYPE, INITIALIZER, instance)
#define HL_SINGLETON_LAZY_INIT_REF(TYPE, INITIALIZER) HL_SINGLETON_LAZY_INIT_(TYPE, INITIALIZER, *instance)

}    // namespace hl