#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    int Bytes = LineByte(bmp.infoHeader.biWidth, bmp.infoHeader.biBitCount) * bmp.infoHeader.biHeight;
    bmp.data = (BYTE*)malloc(Bytes);

    fread(bmp.data, Bytes, 1, file);
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
    int Bytes = LineByte(bmp.infoHeader.biWidth, bmp.infoHeader.biBitCount) * bmp.infoHeader.biHeight;
    fwrite(bmp.data, Bytes, 1, file);
    fclose(file);
}

void bmpDelete(bmp_HL* bmp)
{
    free(bmp->palette);
    free(bmp->data);
    bmp->palette = NULL;
    bmp->data    = NULL;
}

// 将(b, g, r)转换为灰度值
inline static BYTE gray(BYTE b, BYTE g, BYTE r)
{
    return (BYTE)(0.299 * r + 0.587 * g + 0.114 * b);
}

bmp_HL bmp_24to8_gray(bmp_HL bmp)
{
    if (bmp.infoHeader.biBitCount != 24)
    {
        printf("Error: Only 24-bit true color image can be converted to 8-bit grayscale image\n");
        exit(1);
    }

    bmp_HL output;
    memset(&output, 0, sizeof(bmp_HL));

    // 创建256色灰度调色板
    unsigned int paletteSize = 256;
    PALETTE      palette     = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
    for (int i = 0; i < paletteSize; i++)
    {
        palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
        palette[i].rgbReserved = 0;
    }

    // 创建8位灰度图像数据
    unsigned int dataSize = LineByte(bmp.infoHeader.biWidth, 8) * bmp.infoHeader.biHeight;
    BYTE* data = (BYTE*)malloc(dataSize);

    output.header = bmp.header;
    output.infoHeader = bmp.infoHeader;

    for (int i = 0; i < dataSize; i++)
    {
        // 每个像素点的调色板索引值为灰度值
        data[i] = gray(bmp.data[i * 3], bmp.data[i * 3 + 1], bmp.data[i * 3 + 2]);
    }

    output.header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;// 更新文件大小
    output.header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);// 更新数据偏移
    output.infoHeader.biBitCount = 8;// 更新位数
    output.infoHeader.biSizeImage = dataSize;
    output.palette = palette;
    output.data = data;
    return output;
}

bmp_HL bmp_8_graytoinvert_gray(bmp_HL bmp)
{
    if (bmp.infoHeader.biBitCount != 8)
    {
        printf("Error: Only 8-bit grayscale image can be converted to inverted 8-bit grayscale image\n");
        exit(1);
    }

    bmp_HL output;
    memset(&output, 0, sizeof(bmp_HL)); // 初始化 output

    output.header = bmp.header;
    output.infoHeader = bmp.infoHeader;
    output.palette = (PALETTE)malloc(256);
    memcpy(output.palette, bmp.palette, 1024); // 保留调色板
    output.data = (BYTE*)malloc(bmp.infoHeader.biSizeImage);
    for(int i = 0;i < bmp.infoHeader.biSizeImage; i++) {
        output.data[i] = 255 - bmp.data[i];
    }
    return output;
}