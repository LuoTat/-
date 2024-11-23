#pragma once

#include "utils.h"

typedef enum
{
    BMP_RGB       = 0,
    BMP_RLE8      = 1,
    BMP_RLE4      = 2,
    BMP_BITFIELDS = 3
} BmpCompression;

typedef struct
{
    int            width;                   // 图像宽度
    int            height;                  // 图像高度
    int            type;                    // 元素类型
    const char*    filename;                // 文件名
    FILE*          file;                    // 文件指针
    PaletteEntry   palette[256];            // 调色板
    unsigned short bpp;                     // 每个像素的位数
    int            offset;                  // 数据偏移
    BmpCompression rle_code;                // RLE压缩类型
    unsigned int   rgba_mask[4];            // RGBA掩码
    int            rgba_bit_offset[4];      // RGBA位偏移
    float          rgba_scale_factor[4];    // RGBA缩放因子
} BmpDecoder;

BmpDecoder initBmpDecoder();
void       setSource(BmpDecoder* decoder, const char* filename);
bool       readHeader(BmpDecoder* decoder);
bool       readData(BmpDecoder* decoder, Mat* img);
void       close(BmpDecoder* decoder);

typedef struct
{
    const char* filename;    // 文件名
} BmpEncoder;

bool isFormatSupported(int depth);
void setDestination(BmpEncoder* encoder, const char* filename);
bool write(const BmpEncoder* encoder, const Mat* img);