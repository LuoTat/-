#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>

bmp_HL bmpRead(const char* filename)
{
    bmp_HL bmp;
    FILE*  file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    fread(&bmp.header, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&bmp.infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    // 判断是否有调色板
    if (bmp.infoHeader.biBitCount == 24) bmp.palette = NULL;
    else
    {
        unsigned int paletteSize = PaletteSize(bmp.infoHeader.biClrUsed, bmp.infoHeader.biBitCount);
        bmp.palette              = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        fread(bmp.palette, sizeof(RGBQUAD), paletteSize, file);
    }
    bmp.data = (unsigned char*)malloc(bmp.infoHeader.biSizeImage);
    fread(bmp.data, bmp.infoHeader.biSizeImage, 1, file);
    fclose(file);
    return bmp;
}

void bmpWrite(const char* filename, bmp_HL bmp)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    fwrite(&bmp.header, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bmp.infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    if (bmp.palette != NULL)
    {
        unsigned int paletteSize = PaletteSize(bmp.infoHeader.biClrUsed, bmp.infoHeader.biBitCount);
        fwrite(bmp.palette, sizeof(RGBQUAD), paletteSize, file);
    }
    fwrite(bmp.data, bmp.infoHeader.biSizeImage, 1, file);
    fclose(file);
}

bmp_HL bmpCopy(bmp_HL bmp)
{
    bmp_HL copy;
    copy.header     = bmp.header;
    copy.infoHeader = bmp.infoHeader;
    if (bmp.palette != NULL)
    {
        unsigned int paletteSize = PaletteSize(bmp.infoHeader.biClrUsed, bmp.infoHeader.biBitCount);
        copy.palette             = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        for (int i = 0; i < paletteSize; i++) copy.palette[i] = bmp.palette[i];
    }
    else copy.palette = NULL;
    copy.data = (unsigned char*)malloc(bmp.infoHeader.biSizeImage);
    for (int i = 0; i < bmp.infoHeader.biSizeImage; i++) copy.data[i] = bmp.data[i];
    return copy;
}

void bmpDelete(bmp_HL* bmp)
{
    free(bmp->palette);
    free(bmp->data);
    bmp->palette = NULL;
    bmp->data    = NULL;
}

// 将(b, g, r)转换为灰度值
inline static unsigned char rgbtogray(unsigned char b, unsigned char g, unsigned char r)
{
    return (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
}

bmp_HL bmp_24to8_gray(bmp_HL bmp)
{
    if (bmp.infoHeader.biBitCount != 24)
    {
        printf("Error: Only 24-bit true color image can be converted to 8-bit grayscale image\n");
        exit(1);
    }

    // 复制bmp图像
    bmp                      = bmpCopy(bmp);

    // 创建256色灰度调色板
    unsigned int paletteSize = 256;
    PALETTE      palette     = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
    for (unsigned int i = 0; i < paletteSize; i++)
    {
        palette[i].rgbBlue     = i;
        palette[i].rgbGreen    = i;
        palette[i].rgbRed      = i;
        palette[i].rgbReserved = 0;
    }

    // 创建8位灰度图像数据
    unsigned int   dataSize = LineByte(bmp.infoHeader.biWidth, 8) * bmp.infoHeader.biHeight;
    unsigned char* data     = (unsigned char*)malloc(dataSize);

    for (unsigned int i = 0; i < dataSize; i++)
    {
        // 每个像素点的调色板索引值为灰度值
        data[i] = rgbtogray(bmp.data[i * 3], bmp.data[i * 3 + 1], bmp.data[i * 3 + 2]);
    }

    bmp.header.bfSize          = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;    // 更新文件大小
    bmp.header.bfOffBits       = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);               // 更新数据偏移
    bmp.infoHeader.biBitCount  = 8;                                                                                                 // 更新位数
    bmp.infoHeader.biSizeImage = dataSize;                                                                                          // 更新图像数据区大小
    bmp.infoHeader.biClrUsed   = 0;                                                                                                 // 更新调色板大小
    bmp.palette                = palette;                                                                                           // 更新调色板
    bmp.data                   = data;                                                                                              // 更新数据
    return bmp;
}

bmp_HL bmp_8_grayto8_gray_invert(bmp_HL bmp)
{
    if (bmp.infoHeader.biBitCount != 8)
    {
        printf("Error: Only 8-bit grayscale image can be converted to inverted 8-bit grayscale image\n");
        exit(1);
    }

    // 复制bmp图像
    bmp = bmpCopy(bmp);

    // 修改图像数据区
    for (unsigned int i = 0; i < bmp.infoHeader.biSizeImage; i++) bmp.data[i] = 255 - bmp.data[i];
    return bmp;
}

bmp_HL bmp_24split_rgb_channel(bmp_HL bmp, COLOR color)
{
    if (bmp.infoHeader.biBitCount != 24)
    {
        printf("Error: Only 24-bit true color image can be split to RGB channel\n");
        exit(1);
    }

    // 复制bmp图像
    bmp = bmpCopy(bmp);

    // 修改图像数据区
    for (unsigned int i = 0; i < bmp.infoHeader.biSizeImage; i += 3)
    {
        switch (color)
        {
            case RED :
                bmp.data[i]     = 0;
                bmp.data[i + 1] = 0;
                break;
            case GREEN :
                bmp.data[i]     = 0;
                bmp.data[i + 2] = 0;
                break;
            case BLUE :
                bmp.data[i + 1] = 0;
                bmp.data[i + 2] = 0;
                break;
        }
    }
    return bmp;
}