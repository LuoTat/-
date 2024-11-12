#include "BmpEncoder.h"
#include <stdio.h>
#include <stdlib.h>

void setDestination(BmpEncoder* encoder, const char* filename)
{
    encoder->filename = filename;
}

void write(const BmpEncoder* encoder, const Mat* img)
{
    int width    = img->cols;
    int height   = img->rows;
    int channels = img->channels;                  // 这里每个通道的大小都是1字节，因为使用BaseImageEncoder时已经固定为CV_8U
    int fileStep = (width * channels + 3) & -4;    // 得到4字节对齐的宽度
    int zeropad  = 0;

    FILE* fp     = fopen(encoder->filename, "wb");
    if (!fp)
    {
        printf("Error: could not open file %s for writing\n", encoder->filename);
        exit(1);
    }

    int          bitmapHeaderSize = 40;
    int          paletteSize      = channels > 1 ? 0 : 1024;
    int          headerSize       = 14 /* fileheader */ + bitmapHeaderSize + paletteSize;
    size_t       fileSize         = (size_t)fileStep * height + headerSize;
    short        biPlanes         = 1;
    short        biBitCount       = channels << 3;
    PaletteEntry palette[256];

    // 写入文件头
    fwrite("BM", sizeof(short), 1, fp);         // 写入bfType
    fwrite(&fileSize, sizeof(int), 1, fp);      // 写入bfSize
    fwrite(&zeropad, sizeof(short), 2, fp);     // 写入bfReserved1和bfReserved2
    fwrite(&headerSize, sizeof(int), 1, fp);    // 写入bfOffBits

    // 写入信息头
    fwrite(&bitmapHeaderSize, sizeof(unsigned int), 1, fp);    // 写入biSize
    fwrite(&width, sizeof(int), 1, fp);                        // 写入biWidth
    fwrite(&height, sizeof(int), 1, fp);                       // 写入biHeight
    fwrite(&biPlanes, sizeof(short), 1, fp);                   // 写入biPlanes
    fwrite(&biBitCount, sizeof(short), 1, fp);                 // 写入biBitCount
    fwrite(&zeropad, sizeof(int), 6, fp);                      // 写入biCompression, biSizeImage, biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImportant

    if (channels == 1)
    {
        for (int i = 0; i < 256; ++i)
        {
            palette[i].rgbBlue     = i;
            palette[i].rgbGreen    = i;
            palette[i].rgbRed      = i;
            palette[i].rgbReserved = 0;
        }
        fwrite(palette, sizeof(palette), 1, fp);
    }

    width *= channels;
    for (int y = 0; y < height; ++y)
    {
        fwrite(img->data + y * img->step, width, 1, fp);
        if (fileStep > width) fwrite(&zeropad, sizeof(char), fileStep - width, fp);
    }

    fclose(fp);
}
