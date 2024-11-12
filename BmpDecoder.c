#include "BmpDecoder.h"
#include <stdio.h>
#include <stdlib.h>

BmpDecoder initBmpDecoder()
{
    BmpDecoder decoder = {
        .height = 0,
        .width  = 0,
        .bpp    = 0,
        .offset = -1,
    };
    return decoder;
}

void setSource(BmpDecoder* decoder, const char* filename)
{
    decoder->filename = filename;
}

void readHeader(BmpDecoder* decoder)
{
    decoder->file = fopen(decoder->filename, "rb");
    if (decoder->file == NULL)
    {
        printf("Error: %s cannot be opened\n", decoder->filename);
        exit(1);
    }

    fseek(decoder->file, 10, SEEK_CUR);                        // 跳过bfType，bfSize，bfReserved1，bfReserved2
    fread(&decoder->offset, sizeof(int), 1, decoder->file);    // 读取bfOffBits

    fseek(decoder->file, 4, SEEK_CUR);                         // 跳过biSize
    fread(&decoder->width, sizeof(int), 1, decoder->file);     // 读取biWidth
    fread(&decoder->height, sizeof(int), 1, decoder->file);    // 读取biHeight

    fseek(decoder->file, 2, SEEK_CUR);                         // 跳过biPlanes
    fread(&decoder->bpp, sizeof(short), 1, decoder->file);     // 读取biBitCount
}

void readData(const BmpDecoder* decoder, Mat* img)
{
    unsigned char* data      = img->data;
    int            step      = (int)img->step;
    int            src_pitch = ((decoder->width * decoder->bpp + 7) / 8 + 3) & -4;    // 得到4字节对齐的宽度
    int            skip      = src_pitch - step;

    fseek(decoder->file, decoder->offset, SEEK_SET);
    switch (decoder->bpp)
    {
        case 8 :
            for (int y = 0; y < decoder->height; ++y, data += step)
            {
                fread(data, sizeof(char), decoder->width, decoder->file);
                fseek(decoder->file, skip, SEEK_CUR);    // skip padding
            }
            break;
        case 24 :
            for (int y = 0; y < decoder->height; ++y, data += step)
            {
                // 这里之所以用 decoder->width * 3 而不是 step，因为不保证step正好是decoder->width * 3
                fread(data, sizeof(char), decoder->width * 3, decoder->file);
                fseek(decoder->file, skip, SEEK_CUR);    // skip padding
            }
            break;
        default :
            printf("Error: %d bpp is not supported\n", decoder->bpp);
            exit(1);
    }
}

void close(BmpDecoder* decoder)
{
    fclose(decoder->file);
}