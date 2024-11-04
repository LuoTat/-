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
    fread(&bmp.header, sizeof(bmpHEADER), 1, file);
    fread(&bmp.infoHeader, sizeof(bmpINFOHEADER), 1, file);

    // 判断是否有调色板
    if (bmp.infoHeader.biBitCount == 24)    // 24位真彩色图像没有调色板
    {
        bmp.palette = NULL;
    }
    else
    {
        // 计算调色板大小
        unsigned int paletteSize = bmp.infoHeader.biClrUsed ? bmp.infoHeader.biClrUsed : 1 << bmp.infoHeader.biBitCount;
        bmp.palette              = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
        fread(bmp.palette, sizeof(RGBQUAD), paletteSize, file);
    }
    bmp.data = (unsigned char*)malloc(bmp.infoHeader.biSizeImage);
    fread(bmp.data, bmp.infoHeader.biSizeImage, 1, file);
    fclose(file);
    return bmp;
}