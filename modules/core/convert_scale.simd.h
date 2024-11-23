#pragma once

#include "precomp.h"
#include "saturate.h"

BinaryFunc getConvertScaleFunc(int sdepth, int ddepth);


#define cvt_32f(suffix_d, Ts, Td, src, sstep, dst, dstep, width, height, a, b)     \
    do                                                                             \
    {                                                                              \
        sstep /= sizeof(src[0]);                                                   \
        dstep /= sizeof(dst[0]);                                                   \
        for (int i = 0; i < height; ++i, src += sstep, dst += dstep)               \
        {                                                                          \
            for (int j = 0; j < width; ++j)                                        \
                dst[j] = saturate_cast32f##suffix_d(src[j] * (float)a + (float)b); \
        }                                                                          \
    }                                                                              \
    while (0)

#define cvt_64f(suffix_d, Ts, Td, src, sstep, dst, dstep, width, height, a, b)       \
    do                                                                               \
    {                                                                                \
        sstep /= sizeof(src[0]);                                                     \
        dstep /= sizeof(dst[0]);                                                     \
        for (int i = 0; i < height; ++i, src += sstep, dst += dstep)                 \
        {                                                                            \
            for (int j = 0; j < width; ++j)                                          \
                dst[j] = saturate_cast64f##suffix_d(src[j] * (double)a + (double)b); \
        }                                                                            \
    }                                                                                \
    while (0)

#define DEF_CVT_SCALE_FUNC(suffix_s, suffix_d, cvtfunc, Ts, Td, Tw)                                                                                                                         \
    static void cvtScale##suffix_s##suffix_d(const unsigned char* src_, size_t sstep, const unsigned char*, size_t, unsigned char* dst_, size_t dstep, int width, int height, void* scale_) \
    {                                                                                                                                                                                       \
        const Ts* src   = (const Ts*)src_;                                                                                                                                                  \
        Td*       dst   = (Td*)dst_;                                                                                                                                                        \
        double*   scale = (double*)scale_;                                                                                                                                                  \
        cvtfunc(suffix_d, Ts, Td, src, sstep, dst, dstep, width, height, (Tw)scale[0], (Tw)scale[1]);                                                                                       \
    }

////////////////////// 8u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(, 8u, cvt_32f, unsigned char, unsigned char, float)
DEF_CVT_SCALE_FUNC(8u, 8s, cvt_32f, unsigned char, char, float)
DEF_CVT_SCALE_FUNC(8u, 16u, cvt_32f, unsigned char, unsigned short, float)
DEF_CVT_SCALE_FUNC(8u, 16s, cvt_32f, unsigned char, short, float)
DEF_CVT_SCALE_FUNC(8u, 32u, cvt_32f, unsigned char, unsigned int, float)
DEF_CVT_SCALE_FUNC(8u, 32s, cvt_32f, unsigned char, int, float)
DEF_CVT_SCALE_FUNC(8u, 32f, cvt_32f, unsigned char, float, float)
DEF_CVT_SCALE_FUNC(8u, 64f, cvt_64f, unsigned char, double, double)

////////////////////// 8s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(8s, 8u, cvt_32f, char, unsigned char, float)
DEF_CVT_SCALE_FUNC(, 8s, cvt_32f, char, char, float)
DEF_CVT_SCALE_FUNC(8s, 16u, cvt_32f, char, unsigned short, float)
DEF_CVT_SCALE_FUNC(8s, 16s, cvt_32f, char, short, float)
DEF_CVT_SCALE_FUNC(8s, 32u, cvt_32f, char, unsigned int, float)
DEF_CVT_SCALE_FUNC(8s, 32s, cvt_32f, char, int, float)
DEF_CVT_SCALE_FUNC(8s, 32f, cvt_32f, char, float, float)
DEF_CVT_SCALE_FUNC(8s, 64f, cvt_64f, char, double, double)

////////////////////// 16u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(16u, 8u, cvt_32f, unsigned short, unsigned char, float)
DEF_CVT_SCALE_FUNC(16u, 8s, cvt_32f, unsigned short, char, float)
DEF_CVT_SCALE_FUNC(, 16u, cvt_32f, unsigned short, unsigned short, float)
DEF_CVT_SCALE_FUNC(16u, 16s, cvt_32f, unsigned short, short, float)
DEF_CVT_SCALE_FUNC(16u, 32u, cvt_32f, unsigned short, unsigned int, float)
DEF_CVT_SCALE_FUNC(16u, 32s, cvt_32f, unsigned short, int, float)
DEF_CVT_SCALE_FUNC(16u, 32f, cvt_32f, unsigned short, float, float)
DEF_CVT_SCALE_FUNC(16u, 64f, cvt_64f, unsigned short, double, double)

////////////////////// 16s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(16s, 8u, cvt_32f, short, unsigned char, float)
DEF_CVT_SCALE_FUNC(16s, 8s, cvt_32f, short, char, float)
DEF_CVT_SCALE_FUNC(16s, 16u, cvt_32f, short, unsigned short, float)
DEF_CVT_SCALE_FUNC(, 16s, cvt_32f, short, short, float)
DEF_CVT_SCALE_FUNC(16s, 32u, cvt_32f, short, unsigned int, float)
DEF_CVT_SCALE_FUNC(16s, 32s, cvt_32f, short, int, float)
DEF_CVT_SCALE_FUNC(16s, 32f, cvt_32f, short, float, float)
DEF_CVT_SCALE_FUNC(16s, 64f, cvt_64f, short, double, double)

////////////////////// 32u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32u, 8u, cvt_32f, unsigned int, unsigned char, float)
DEF_CVT_SCALE_FUNC(32u, 8s, cvt_32f, unsigned int, char, float)
DEF_CVT_SCALE_FUNC(32u, 16u, cvt_32f, unsigned int, unsigned short, float)
DEF_CVT_SCALE_FUNC(32u, 16s, cvt_32f, unsigned int, short, float)
DEF_CVT_SCALE_FUNC(, 32u, cvt_32f, unsigned int, unsigned int, float)
DEF_CVT_SCALE_FUNC(32u, 32s, cvt_32f, unsigned int, int, float)
DEF_CVT_SCALE_FUNC(32u, 32f, cvt_32f, unsigned int, float, float)
DEF_CVT_SCALE_FUNC(32u, 64f, cvt_64f, unsigned int, double, double)

////////////////////// 32s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32s, 8u, cvt_32f, int, unsigned char, float)
DEF_CVT_SCALE_FUNC(32s, 8s, cvt_32f, int, char, float)
DEF_CVT_SCALE_FUNC(32s, 16u, cvt_32f, int, unsigned short, float)
DEF_CVT_SCALE_FUNC(32s, 16s, cvt_32f, int, short, float)
DEF_CVT_SCALE_FUNC(32s, 32u, cvt_32f, int, unsigned int, float)
DEF_CVT_SCALE_FUNC(, 32s, cvt_32f, int, int, float)
DEF_CVT_SCALE_FUNC(32s, 32f, cvt_32f, int, float, float)
DEF_CVT_SCALE_FUNC(32s, 64f, cvt_64f, int, double, double)

////////////////////// 32f -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32f, 8u, cvt_32f, float, unsigned char, float)
DEF_CVT_SCALE_FUNC(32f, 8s, cvt_32f, float, char, float)
DEF_CVT_SCALE_FUNC(32f, 16u, cvt_32f, float, unsigned short, float)
DEF_CVT_SCALE_FUNC(32f, 16s, cvt_32f, float, short, float)
DEF_CVT_SCALE_FUNC(32f, 32u, cvt_32f, float, unsigned int, float)
DEF_CVT_SCALE_FUNC(32f, 32s, cvt_32f, float, int, float)
DEF_CVT_SCALE_FUNC(, 32f, cvt_32f, float, float, float)
DEF_CVT_SCALE_FUNC(32f, 64f, cvt_64f, float, double, double)

////////////////////// 64f -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(64f, 8u, cvt_32f, double, unsigned char, float)
DEF_CVT_SCALE_FUNC(64f, 8s, cvt_32f, double, char, float)
DEF_CVT_SCALE_FUNC(64f, 16u, cvt_32f, double, unsigned short, float)
DEF_CVT_SCALE_FUNC(64f, 16s, cvt_32f, double, short, float)
DEF_CVT_SCALE_FUNC(64f, 32u, cvt_64f, double, unsigned int, double)
DEF_CVT_SCALE_FUNC(64f, 32s, cvt_64f, double, int, double)
DEF_CVT_SCALE_FUNC(64f, 32f, cvt_64f, double, float, double)
DEF_CVT_SCALE_FUNC(, 64f, cvt_64f, double, double, double)

BinaryFunc convertScaleFunc_cpu_baseline(int sdepth, int ddepth)
{
    static BinaryFunc cvtScaleTab[HL_DEPTH_MAX][HL_DEPTH_MAX] = {
        {(cvtScale8u), (cvtScale8u8s), (cvtScale8u16u), (cvtScale8u16s), (cvtScale8u32u), (cvtScale8u32s), (cvtScale8u32f), (cvtScale8u64f)},
        {(cvtScale8s8u), (cvtScale8s), (cvtScale8s16u), (cvtScale8s16s), (cvtScale8s32u), (cvtScale8s32s), (cvtScale8s32f), (cvtScale8s64f)},
        {(cvtScale16u8u), (cvtScale16u8s), (cvtScale16u), (cvtScale16u16s), (cvtScale16u32u), (cvtScale16u32s), (cvtScale16u32f), (cvtScale16u64f)},
        {(cvtScale16s8u), (cvtScale16s8s), (cvtScale16s16u), (cvtScale16s), (cvtScale16s32u), (cvtScale16s32s), (cvtScale16s32f), (cvtScale16s64f)},
        {(cvtScale32u8u), (cvtScale32u8s), (cvtScale32u16u), (cvtScale32u16s), (cvtScale32u), (cvtScale32u32s), (cvtScale32u32f), (cvtScale32u64f)},
        {(cvtScale32s8u), (cvtScale32s8s), (cvtScale32s16u), (cvtScale32s16s), (cvtScale32s32u), (cvtScale32s), (cvtScale32s32f), (cvtScale32s64f)},
        {(cvtScale32f8u), (cvtScale32f8s), (cvtScale32f16u), (cvtScale32f16s), (cvtScale32f32u), (cvtScale32f32s), (cvtScale32f), (cvtScale32f64f)},
        {(cvtScale64f8u), (cvtScale64f8s), (cvtScale64f16u), (cvtScale64f16s), (cvtScale64f32u), (cvtScale64f32s), (cvtScale64f32f), (cvtScale64f)}};
    return cvtScaleTab[HL_MAT_DEPTH(sdepth)][HL_MAT_DEPTH(ddepth)];
}