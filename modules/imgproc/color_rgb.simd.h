#pragma once

#include "precomp.h"
#include "color.simd_helpers.h"

void RGB2Gray8uc3(const void* _src, void* _dst, int n)
{
    const unsigned char* src = (const unsigned char*)_src;
    unsigned char*       dst = (unsigned char*)_dst;
    int                  scn = 3;
    short                cb = RY15, cg = GY15, cr = BY15;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
    {
        int           b = src[0], g = src[1], r = src[2];
        unsigned char y = (unsigned char)HL_DESCALE(b * cb + g * cg + r * cr, gray_shift);
        dst[0]          = y;
    }
}

void RGB2Gray8uc4(const void* _src, void* _dst, int n)
{
    const unsigned char* src = (const unsigned char*)_src;
    unsigned char*       dst = (unsigned char*)_dst;
    int                  scn = 4;
    short                cb = RY15, cg = GY15, cr = BY15;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
    {
        int           b = src[0], g = src[1], r = src[2];
        unsigned char y = (unsigned char)HL_DESCALE(b * cb + g * cg + r * cr, gray_shift);
        dst[0]          = y;
    }
}

void RGB2Gray16uc3(const void* _src, void* _dst, int n)
{
    const unsigned short* src = (const unsigned short*)_src;
    unsigned short*       dst = (unsigned short*)_dst;
    int                   scn = 3;
    short                 cb = RY15, cg = GY15, cr = BY15;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
    {
        int            b = src[0], g = src[1], r = src[2];
        unsigned short d = (unsigned short)HL_DESCALE((unsigned int)(b * cb + g * cg + r * cr), gray_shift);
        dst[0]           = d;
    }
}

void RGB2Gray16uc4(const void* _src, void* _dst, int n)
{
    const unsigned short* src = (const unsigned short*)_src;
    unsigned short*       dst = (unsigned short*)_dst;
    int                   scn = 4;
    short                 cb = RY15, cg = GY15, cr = BY15;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
    {
        int            b = src[0], g = src[1], r = src[2];
        unsigned short d = (unsigned short)HL_DESCALE((unsigned int)(b * cb + g * cg + r * cr), gray_shift);
        dst[0]           = d;
    }
}

void RGB2Gray32fc3(const void* _src, void* _dst, int n)
{
    const float* src = (const float*)_src;
    float*       dst = (float*)_dst;
    int          scn = 3;
    float        cb = R2YF, cg = G2YF, cr = B2YF;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
        dst[0] = src[0] * cb + src[1] * cg + src[2] * cr;
}

void RGB2Gray32fc4(const void* _src, void* _dst, int n)
{
    const float* src = (const float*)_src;
    float*       dst = (float*)_dst;
    int          scn = 4;
    float        cb = R2YF, cg = G2YF, cr = B2YF;
    for (int i = 0; i < n; ++i, src += scn, ++dst)
        dst[0] = src[0] * cb + src[1] * cg + src[2] * cr;
}

#define RGB2Gray(suffix, scn) RGB2Gray##suffix##scn

// 8u, 16u, 32f
void cvtBGRtoGray_cpu_baseline(const unsigned char* src_data, size_t src_step, unsigned char* dst_data, size_t dst_step, int width, int height, int depth, int scn)
{
    if (depth == HL_8U)
    {
        if (scn == 3)
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(8u, c3));
        else
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(8u, c4));
    }
    else if (depth == HL_16U)
    {
        if (scn == 3)
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(16u, c3));
        else
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(16u, c4));
    }
    else
    {
        if (scn == 3)
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(32f, c3));
        else
            CvtColorLoop(src_data, src_step, dst_data, dst_step, width, height, RGB2Gray(32f, c4));
    }
}