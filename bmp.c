#include "bmp.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LineByte(width, bitCount)          ((((width) * (bitCount) + 31) >> 5) << 2)
#define PaletteSize(biClrUsed, biBitCount) ((biClrUsed) ? (biClrUsed) : 1 << (biBitCount))

typedef struct
{
    unsigned short bfType;             // 文件类型
    unsigned int   bfSize;             // 文件大小（包括文件头）
    unsigned short bfReserved1;        // 保留字1
    unsigned short bfReserved2;        // 保留字2
    unsigned int   bfOffBits;          // 从文件头到实际位图数据的偏移字节数
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct
{
    unsigned int   biSize;             // 信息头大小
    int            biWidth;            // 图像宽度
    int            biHeight;           // 图像高度
    unsigned short biPlanes;           // 位平面数，必须为1
    unsigned short biBitCount;         // 每个像素的位数
    unsigned int   biCompression;      // 压缩类型
    unsigned int   biSizeImage;        // 图像大小
    int            biXPelsPerMeter;    // 水平分辨率
    int            biYPelsPerMeter;    // 垂直分辨率
    unsigned int   biClrUsed;          // 使用的颜色数
    unsigned int   biClrImportant;     // 重要的颜色数
} BITMAPINFOHEADER;

typedef struct
{
    unsigned char rgbBlue;             // 蓝色分量
    unsigned char rgbGreen;            // 绿色分量
    unsigned char rgbRed;              // 红色分量
    unsigned char rgbReserved;         // 保留字
} RGBQUAD;

typedef RGBQUAD* PALETTE;

typedef struct
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER infoHeader;
    PALETTE          palette;
    unsigned char*   data;
} BMP_HL;

static BMP_HL copyBMP(BMP_HL* bmp)
{
    BMP_HL copy;
    copy.header     = bmp->header;
    copy.infoHeader = bmp->infoHeader;
    if (bmp->palette != NULL)
    {
        unsigned int paletteSize = PaletteSize(bmp->infoHeader.biClrUsed, bmp->infoHeader.biBitCount);
        copy.palette             = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        memcpy(copy.palette, bmp->palette, paletteSize * sizeof(RGBQUAD));
    }
    else copy.palette = NULL;
    copy.data = (unsigned char*)malloc(bmp->infoHeader.biSizeImage);
    memcpy(copy.data, bmp->data, bmp->infoHeader.biSizeImage);
    return copy;
}

static void deleteBMP(BMP_HL* bmp)
{
    free(bmp->palette);
    free(bmp->data);
    bmp->palette = NULL;
    bmp->data    = NULL;
}

Mat imread(const char* filename)
{
    BMP_HL bmp;

    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    // 读取文件头和信息头
    fread(&bmp.header, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&bmp.infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    if (bmp.header.bfType != 0x4D42)
    {
        printf("Error: Not a BMP file\n");
        exit(1);
    }

    int            rows     = bmp.infoHeader.biHeight;
    int            cols     = bmp.infoHeader.biWidth;
    unsigned short channels = bmp.infoHeader.biBitCount >> 3;
    unsigned int   step     = LineByte(cols, bmp.infoHeader.biBitCount);

    // 读取调色板
    if (channels == 1)
    {
        unsigned int paletteSize = PaletteSize(bmp.infoHeader.biClrUsed, bmp.infoHeader.biBitCount);
        bmp.palette              = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        fread(bmp.palette, sizeof(RGBQUAD), paletteSize, file);
    }
    else bmp.palette = NULL;

    // 直接跳至数据区
    fseek(file, bmp.header.bfOffBits, SEEK_SET);

    // 读取图像数据
    unsigned char* data = (unsigned char*)malloc(bmp.infoHeader.biSizeImage);
    fread(data, bmp.infoHeader.biSizeImage, 1, file);
    fclose(file);

    Mat mat;

    mat = createMat(rows, cols, channels, sizeof(unsigned char));
    for (int i = 0; i < bmp.infoHeader.biHeight; ++i)
    {
        memcpy(mat.data + i * mat.step, data + i * step, mat.step);
    }

    deleteBMP(&bmp);
    return mat;
}

static void BMPwrite(const char* filename, BMP_HL* bmp)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    fwrite(&bmp->header, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bmp->infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    if (bmp->palette != NULL) fwrite(bmp->palette, sizeof(RGBQUAD), 256, file);
    fwrite(bmp->data, bmp->infoHeader.biSizeImage, 1, file);
    fclose(file);
}

static BMP_HL Mat2BMP(Mat* mat)
{
    BMP_HL bmp;

    unsigned int step              = LineByte(mat->cols, mat->elemSize * CHAR_BIT);
    unsigned int dataSize          = mat->rows * step;
    unsigned int paletteSize       = (mat->channels != 1) ? 0 : 256;

    bmp.header.bfType              = 0x4D42;
    bmp.header.bfSize              = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;
    bmp.header.bfReserved1         = 0;
    bmp.header.bfReserved2         = 0;
    bmp.header.bfOffBits           = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);

    bmp.infoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmp.infoHeader.biWidth         = mat->cols;
    bmp.infoHeader.biHeight        = mat->rows;
    bmp.infoHeader.biPlanes        = 1;
    bmp.infoHeader.biBitCount      = mat->channels * CHAR_BIT;
    bmp.infoHeader.biCompression   = 0;
    bmp.infoHeader.biSizeImage     = dataSize;
    bmp.infoHeader.biXPelsPerMeter = 0;
    bmp.infoHeader.biYPelsPerMeter = 0;
    bmp.infoHeader.biClrUsed       = 0;
    bmp.infoHeader.biClrImportant  = 0;

    if (mat->channels == 1)
    {
        bmp.palette = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        for (unsigned int i = 0; i < paletteSize; i++)
        {
            bmp.palette[i].rgbBlue     = i;
            bmp.palette[i].rgbGreen    = i;
            bmp.palette[i].rgbRed      = i;
            bmp.palette[i].rgbReserved = 0;
        }
    }
    else bmp.palette = NULL;

    bmp.data = (unsigned char*)malloc(dataSize);
    for (int i = 0; i < mat->rows; ++i)
    {
        memcpy(bmp.data + i * step, mat->data + i * mat->step, mat->step);
    }
    return bmp;
}

void imwrite(const char* filename, Mat* img)
{
    BMP_HL bmp = Mat2BMP(img);
    BMPwrite(filename, &bmp);
    deleteBMP(&bmp);
}