#pragma once

#include "mat.h"
#include <stdio.h>

typedef struct
{
    unsigned char rgbBlue;        // 蓝色分量
    unsigned char rgbGreen;       // 绿色分量
    unsigned char rgbRed;         // 红色分量
    unsigned char rgbReserved;    // 保留字
} PaletteEntry;

typedef struct
{
    const char* filename;         // 文件名
} BmpEncoder;

void setDestination(BmpEncoder* encoder, const char* filename);
void write(const BmpEncoder* encoder, const Mat* img);