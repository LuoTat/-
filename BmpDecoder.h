#pragma once

#include "mat.h"
#include <stdio.h>

typedef struct
{
    int            width;       // 图像宽度
    int            height;      // 图像高度
    const char*    filename;    // 文件名
    FILE*          file;        // 文件指针
    unsigned short bpp;         // 每个像素的位数
    int            offset;      // 数据偏移
} BmpDecoder;

BmpDecoder initBmpDecoder();
void       setSource(BmpDecoder* decoder, const char* filename);
void       readHeader(BmpDecoder* decoder);
void       readData(const BmpDecoder* decoder, Mat* img);
void       close(BmpDecoder* decoder);