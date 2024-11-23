#pragma once

#include "precomp.h"
#include "saturate.h"

BinaryFunc getConvertFunc(int sdepth, int ddepth);

#define cvt_(suffix, Ts, Td, src, sstep, dst, dstep, width, height)  \
    do                                                               \
    {                                                                \
        sstep /= sizeof(src[0]);                                     \
        dstep /= sizeof(dst[0]);                                     \
        for (int i = 0; i < height; ++i, src += sstep, dst += dstep) \
        {                                                            \
            for (int j = 0; j < width; ++j)                          \
                dst[j] = saturate_cast##suffix(src[j]);              \
        }                                                            \
    }                                                                \
    while (0)

static void cvtCopy(const unsigned char* src, size_t sstep, unsigned char* dst, size_t dstep, int width, int height, size_t elemsize)
{
    size_t len = width * elemsize;
    for (int i = 0; i < height; i++, src += sstep, dst += dstep)
    {
        memcpy(dst, src, len);
    }
}

#define DEF_CVT_FUNC(suffix, cvtfunc, Ts, Td)                                                                                                                       \
    static void cvt##suffix(const unsigned char* src_, size_t sstep, const unsigned char*, size_t, unsigned char* dst_, size_t dstep, int width, int height, void*) \
    {                                                                                                                                                               \
        const Ts* src = (const Ts*)src_;                                                                                                                            \
        Td*       dst = (Td*)dst_;                                                                                                                                  \
        cvtfunc(suffix, Ts, Td, src, sstep, dst, dstep, width, height);                                                                                             \
    }

////////////////////// 8u -> ... ////////////////////////

DEF_CVT_FUNC(8u8s, cvt_, unsigned char, char)
DEF_CVT_FUNC(8u16u, cvt_, unsigned char, unsigned short)
DEF_CVT_FUNC(8u16s, cvt_, unsigned char, short)
DEF_CVT_FUNC(8u32u, cvt_, unsigned char, unsigned int)
DEF_CVT_FUNC(8u32s, cvt_, unsigned char, int)
DEF_CVT_FUNC(8u32f, cvt_, unsigned char, float)
DEF_CVT_FUNC(8u64f, cvt_, unsigned char, double)

////////////////////// 8s -> ... ////////////////////////

DEF_CVT_FUNC(8s8u, cvt_, char, unsigned char)
DEF_CVT_FUNC(8s16u, cvt_, char, unsigned short)
DEF_CVT_FUNC(8s16s, cvt_, char, short)
DEF_CVT_FUNC(8s32u, cvt_, char, unsigned int)
DEF_CVT_FUNC(8s32s, cvt_, char, int)
DEF_CVT_FUNC(8s32f, cvt_, char, float)
DEF_CVT_FUNC(8s64f, cvt_, char, double)

////////////////////// 16u -> ... ////////////////////////

DEF_CVT_FUNC(16u8u, cvt_, unsigned short, unsigned char)
DEF_CVT_FUNC(16u8s, cvt_, unsigned short, char)
DEF_CVT_FUNC(16u16s, cvt_, unsigned short, short)
DEF_CVT_FUNC(16u32u, cvt_, unsigned short, unsigned int)
DEF_CVT_FUNC(16u32s, cvt_, unsigned short, int)
DEF_CVT_FUNC(16u32f, cvt_, unsigned short, float)
DEF_CVT_FUNC(16u64f, cvt_, unsigned short, double)

////////////////////// 16s -> ... ////////////////////////

DEF_CVT_FUNC(16s8u, cvt_, short, unsigned char)
DEF_CVT_FUNC(16s8s, cvt_, short, char)
DEF_CVT_FUNC(16s16u, cvt_, short, unsigned short)
DEF_CVT_FUNC(16s32u, cvt_, short, unsigned int)
DEF_CVT_FUNC(16s32s, cvt_, short, int)
DEF_CVT_FUNC(16s32f, cvt_, short, float)
DEF_CVT_FUNC(16s64f, cvt_, short, double)

////////////////////// 32u -> ... ////////////////////////

DEF_CVT_FUNC(32u8u, cvt_, unsigned int, unsigned char)
DEF_CVT_FUNC(32u8s, cvt_, unsigned int, char)
DEF_CVT_FUNC(32u16u, cvt_, unsigned int, unsigned short)
DEF_CVT_FUNC(32u16s, cvt_, unsigned int, short)
DEF_CVT_FUNC(32u32s, cvt_, unsigned int, int)
DEF_CVT_FUNC(32u32f, cvt_, unsigned int, float)
DEF_CVT_FUNC(32u64f, cvt_, unsigned int, double)

////////////////////// 32s -> ... ////////////////////////

DEF_CVT_FUNC(32s8u, cvt_, int, unsigned char)
DEF_CVT_FUNC(32s8s, cvt_, int, char)
DEF_CVT_FUNC(32s16u, cvt_, int, unsigned short)
DEF_CVT_FUNC(32s16s, cvt_, int, short)
DEF_CVT_FUNC(32s32u, cvt_, int, unsigned int)
DEF_CVT_FUNC(32s32f, cvt_, int, float)
DEF_CVT_FUNC(32s64f, cvt_, int, double)

////////////////////// 32f -> ... ////////////////////////

DEF_CVT_FUNC(32f8u, cvt_, float, unsigned char)
DEF_CVT_FUNC(32f8s, cvt_, float, char)
DEF_CVT_FUNC(32f16u, cvt_, float, unsigned short)
DEF_CVT_FUNC(32f16s, cvt_, float, short)
DEF_CVT_FUNC(32f32u, cvt_, float, unsigned int)
DEF_CVT_FUNC(32f32s, cvt_, float, int)
DEF_CVT_FUNC(32f64f, cvt_, float, double)

////////////////////// 64f -> ... ////////////////////////

DEF_CVT_FUNC(64f8u, cvt_, double, unsigned char)
DEF_CVT_FUNC(64f8s, cvt_, double, char)
DEF_CVT_FUNC(64f16u, cvt_, double, unsigned short)
DEF_CVT_FUNC(64f16s, cvt_, double, short)
DEF_CVT_FUNC(64f32u, cvt_, double, unsigned int)
DEF_CVT_FUNC(64f32s, cvt_, double, int)
DEF_CVT_FUNC(64f32f, cvt_, double, float)

///////////// "conversion" w/o conversion ///////////////

static void cvt8(const unsigned char* src, size_t sstep, const unsigned char*, size_t, unsigned char* dst, size_t dstep, int width, int height, void*)
{
    cvtCopy(src, sstep, dst, dstep, width, height, 1);
}

static void cvt16(const unsigned char* src, size_t sstep, const unsigned char*, size_t, unsigned char* dst, size_t dstep, int width, int height, void*)
{
    cvtCopy((const unsigned char*)src, sstep, (unsigned char*)dst, dstep, width, height, 2);
}

static void cvt32(const unsigned char* src, size_t sstep, const unsigned char*, size_t, unsigned char* dst, size_t dstep, int width, int height, void*)
{
    cvtCopy((const unsigned char*)src, sstep, (unsigned char*)dst, dstep, width, height, 4);
}

static void cvt64(const unsigned char* src, size_t sstep, const unsigned char*, size_t, unsigned char* dst, size_t dstep, int width, int height, void*)
{
    cvtCopy((const unsigned char*)src, sstep, (unsigned char*)dst, dstep, width, height, 8);
}

BinaryFunc getConvertFunc_cpu_baseline(int sdepth, int ddepth)
{
    static BinaryFunc cvtTab[HL_DEPTH_MAX][HL_DEPTH_MAX] = {
        {(cvt8), (cvt8u8s), (cvt8u16u), (cvt8u16s), (cvt8u32u), (cvt8u32s), (cvt8u32f), (cvt8u64f)},
        {(cvt8s8u), (cvt8), (cvt8s16u), (cvt8s16s), (cvt8s32u), (cvt8s32s), (cvt8s32f), (cvt8s64f)},
        {(cvt16u8u), (cvt16u8s), (cvt16), (cvt16u16s), (cvt16u32u), (cvt16u32s), (cvt16u32f), (cvt16u64f)},
        {(cvt16s8u), (cvt16s8s), (cvt16s16u), (cvt16), (cvt16s32u), (cvt16s32s), (cvt16s32f), (cvt16s64f)},
        {(cvt32u8u), (cvt32u8s), (cvt32u16u), (cvt32u16s), (cvt32), (cvt32u32s), (cvt32u32f), (cvt32u64f)},
        {(cvt32s8u), (cvt32s8s), (cvt32s16u), (cvt32s16s), (cvt32s32u), (cvt32), (cvt32s32f), (cvt32s64f)},
        {(cvt32f8u), (cvt32f8s), (cvt32f16u), (cvt32f16s), (cvt32f32u), (cvt32f32s), (cvt32), (cvt32f64f)},
        {(cvt64f8u), (cvt64f8s), (cvt64f16u), (cvt64f16s), (cvt64f32u), (cvt64f32s), (cvt64f32f), (cvt64)}};
    return cvtTab[HL_MAT_DEPTH(sdepth)][HL_MAT_DEPTH(ddepth)];
}