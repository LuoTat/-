#include "bmp.h"
#include "bmp_core.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Mat imread(const char* filename)
{
    bmp_HL bmp;

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

    //     case IMREAD_COLOR :    // 读取BGR3通道图像
    //         mat = createMat(rows, cols, 3, 1);
    //         for (int i = 0; i < rows; ++i)
    //         {
    //             for (int j = 0; j < cols; ++j)
    //             {
    //                 unsigned char* mat_pixel = PIXEL(mat, i, j);
    //                 switch (bmp.infoHeader.biBitCount)
    //                 {
    //                     case 32 :    // 舍去alpha通道
    //                         mat_pixel[0] = data[i * step + j * 4 + 0];
    //                         mat_pixel[1] = data[i * step + j * 4 + 1];
    //                         mat_pixel[2] = data[i * step + j * 4 + 2];
    //                         break;
    //                     case 24 :
    //                         mat_pixel[0] = data[i * step + j * 3 + 0];
    //                         mat_pixel[1] = data[i * step + j * 3 + 1];
    //                         mat_pixel[2] = data[i * step + j * 3 + 2];
    //                         break;
    //                     case 8 :    // 伪彩色图像
    //                         mat_pixel[0] = data[i * step + j];
    //                         mat_pixel[1] = data[i * step + j];
    //                         mat_pixel[2] = data[i * step + j];
    //                         break;
    //                     default :
    //                         printf("Error: Unsupported bit count %d\n", bmp.infoHeader.biBitCount);
    //                         exit(1);
    //                 }
    //             }
    //         }
    //         break;
    //     case IMREAD_GRAYSCALE :    // 读取8位灰度图像
    //         mat = createMat(rows, cols, 1, 1);
    //         for (int i = 0; i < rows; ++i)
    //         {
    //             for (int j = 0; j < cols; ++j)
    //             {
    //                 unsigned char* mat_pixel = PIXEL(mat, i, j);
    //                 switch (bmp.infoHeader.biBitCount)
    //                 {
    //                     case 32 :
    //                         *mat_pixel = (unsigned char)(0.114 * data[i * step + j * 4 + 0] + 0.587 * data[i * step + j * 4 + 1] + 0.299 * data[i * step + j * 4 + 2]);
    //                         break;
    //                     case 24 :
    //                         *mat_pixel = (unsigned char)(0.114 * data[i * step + j * 3 + 0] + 0.587 * data[i * step + j * 3 + 1] + 0.299 * data[i * step + j * 3 + 2]);
    //                         break;
    //                     case 8 :
    //                         *mat_pixel = data[i * step + j];
    //                         break;
    //                     default :
    //                         printf("Error: Unsupported bit count %d\n", bmp.infoHeader.biBitCount);
    //                         exit(1);
    //                 }
    //             }
    //         }
    // }

    free(data);
    return mat;
}

bmp_HL mat2bmp(Mat mat)
{
    bmp_HL bmp;

    unsigned int step              = LineByte(mat.cols, mat.elemSize * CHAR_BIT);
    unsigned int dataSize          = mat.rows * step;
    unsigned int paletteSize       = (mat.channels != 1) ? 0 : 256;

    bmp.header.bfType              = 0x4D42;
    bmp.header.bfSize              = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;
    bmp.header.bfReserved1         = 0;
    bmp.header.bfReserved2         = 0;
    bmp.header.bfOffBits           = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);

    bmp.infoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    bmp.infoHeader.biWidth         = mat.cols;
    bmp.infoHeader.biHeight        = mat.rows;
    bmp.infoHeader.biPlanes        = 1;
    bmp.infoHeader.biBitCount      = mat.channels * CHAR_BIT;
    bmp.infoHeader.biCompression   = 0;
    bmp.infoHeader.biSizeImage     = dataSize;
    bmp.infoHeader.biXPelsPerMeter = 0;
    bmp.infoHeader.biYPelsPerMeter = 0;
    bmp.infoHeader.biClrUsed       = 0;
    bmp.infoHeader.biClrImportant  = 0;

    if (mat.channels == 1)
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
    for (int i = 0; i < mat.rows; ++i)
    {
        memcpy(bmp.data + i * step, mat.data + i * mat.step, mat.step);
    }
    return bmp;
}

void bmpwrite(const char* filename, bmp_HL bmp)
{
    FILE* file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    fwrite(&bmp.header, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&bmp.infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    if (bmp.palette != NULL) fwrite(bmp.palette, sizeof(RGBQUAD), 256, file);
    fwrite(bmp.data, bmp.infoHeader.biSizeImage, 1, file);
    fclose(file);
}

void imwrite(const char* filename, Mat mat)
{
    bmp_HL bmp = mat2bmp(mat);
    bmpwrite(filename, bmp);
    free(bmp.palette);
    free(bmp.data);
}

// bmp_HL bmpCopy(bmp_HL bmp)
// {
//     bmp_HL copy;
//     copy.header     = bmp.header;
//     copy.infoHeader = bmp.infoHeader;
//     if (bmp.palette != NULL)
//     {
//         unsigned int paletteSize = PaletteSize(bmp.infoHeader.biClrUsed, bmp.infoHeader.biBitCount);
//         copy.palette             = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
//         for (int i = 0; i < paletteSize; i++) copy.palette[i] = bmp.palette[i];
//     }
//     else copy.palette = NULL;
//     copy.data = (unsigned char*)malloc(bmp.infoHeader.biSizeImage);
//     for (int i = 0; i < bmp.infoHeader.biSizeImage; i++) copy.data[i] = bmp.data[i];
//     return copy;
// }

// void bmpDelete(bmp_HL* bmp)
// {
//     free(bmp->palette);
//     free(bmp->data);
//     bmp->palette = NULL;
//     bmp->data    = NULL;
// }

// // 将(b, g, r)转换为灰度值
// inline static unsigned char rgbtogray(unsigned char b, unsigned char g, unsigned char r)
// {
//     return (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
// }

// bmp_HL bmp_24to8_gray(bmp_HL bmp)
// {
//     if (bmp.infoHeader.biBitCount != 24)
//     {
//         printf("Error: Only 24-bit true color image can be converted to 8-bit grayscale image\n");
//         exit(1);
//     }

//     // 复制bmp图像
//     bmp                      = bmpCopy(bmp);

//     // 创建256色灰度调色板
//     unsigned int paletteSize = 256;
//     PALETTE      palette     = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
//     for (unsigned int i = 0; i < paletteSize; i++)
//     {
//         palette[i].rgbBlue     = i;
//         palette[i].rgbGreen    = i;
//         palette[i].rgbRed      = i;
//         palette[i].rgbReserved = 0;
//     }

//     // 创建8位灰度图像数据
//     unsigned int   dataSize = LineByte(bmp.infoHeader.biWidth, 8) * bmp.infoHeader.biHeight;
//     unsigned char* data     = (unsigned char*)malloc(dataSize);

//     for (unsigned int i = 0; i < dataSize; i++)
//     {
//         // 每个像素点的调色板索引值为灰度值
//         data[i] = rgbtogray(bmp.data[i * 3], bmp.data[i * 3 + 1], bmp.data[i * 3 + 2]);
//     }

//     bmp.header.bfSize          = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;    // 更新文件大小
//     bmp.header.bfOffBits       = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);               // 更新数据偏移
//     bmp.infoHeader.biBitCount  = 8;                                                                                                 // 更新位数
//     bmp.infoHeader.biSizeImage = dataSize;                                                                                          // 更新图像数据区大小
//     bmp.infoHeader.biClrUsed   = 0;                                                                                                 // 更新调色板大小
//     bmp.palette                = palette;                                                                                           // 更新调色板
//     bmp.data                   = data;                                                                                              // 更新数据
//     return bmp;
// }

// bmp_HL bmp_8_grayto8_gray_invert(bmp_HL bmp)
// {
//     if (bmp.infoHeader.biBitCount != 8)
//     {
//         printf("Error: Only 8-bit grayscale image can be converted to inverted 8-bit grayscale image\n");
//         exit(1);
//     }

//     // 复制bmp图像
//     bmp = bmpCopy(bmp);

//     // 修改图像数据区
//     for (unsigned int i = 0; i < bmp.infoHeader.biSizeImage; i++) bmp.data[i] = 255 - bmp.data[i];
//     return bmp;
// }

// bmp_HL bmp_24split_rgb_channel(bmp_HL bmp, COLOR color)
// {
//     if (bmp.infoHeader.biBitCount != 24)
//     {
//         printf("Error: Only 24-bit true color image can be split to RGB channel\n");
//         exit(1);
//     }

//     // 复制bmp图像
//     bmp                      = bmpCopy(bmp);

//     // 创建256色灰度调色板
//     unsigned int paletteSize = 256;
//     PALETTE      palette     = (PALETTE)malloc(paletteSize * sizeof(RGBQUAD));
//     for (unsigned int i = 0; i < paletteSize; i++)
//     {
//         palette[i].rgbBlue     = i;
//         palette[i].rgbGreen    = i;
//         palette[i].rgbRed      = i;
//         palette[i].rgbReserved = 0;
//     }

//     // 创建8位灰度图像数据
//     unsigned int   dataSize = LineByte(bmp.infoHeader.biWidth, 8) * bmp.infoHeader.biHeight;
//     unsigned char* data     = (unsigned char*)malloc(dataSize);

//     for (unsigned int i = 0; i < dataSize; i++)
//     {
//         // 每个像素点的调色板索引值为灰度值
//         data[i] = rgbtogray(bmp.data[i * 3], bmp.data[i * 3 + 1], bmp.data[i * 3 + 2]);
//     }

//     bmp.header.bfSize          = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD) + dataSize;    // 更新文件大小
//     bmp.header.bfOffBits       = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteSize * sizeof(RGBQUAD);               // 更新数据偏移
//     bmp.infoHeader.biBitCount  = 8;                                                                                                 // 更新位数
//     bmp.infoHeader.biSizeImage = dataSize;                                                                                          // 更新图像数据区大小
//     bmp.infoHeader.biClrUsed   = 0;                                                                                                 // 更新调色板大小
//     bmp.palette                = palette;                                                                                           // 更新调色板
//     bmp.data                   = data;                                                                                              // 更新数据
//     return bmp;




//     // 修改图像数据区
//     for (unsigned int i = 0; i < bmp.infoHeader.biSizeImage; i += 3)
//     {
//         switch (color)
//         {
//             case RED :
//                 if (bmp.data[i + 2] != 0)
//                 {
//                     bmp.data[i]     = 255;
//                     bmp.data[i + 1] = 255;
//                     bmp.data[i + 2] = 255;
//                 }
//                 else
//                 {
//                     bmp.data[i]     = 0;
//                     bmp.data[i + 1] = 0;
//                     bmp.data[i + 2] = 0;
//                 }
//                 break;
//             case GREEN :
//                 if (bmp.data[i + 1] != 0)
//                 {
//                     bmp.data[i]     = 255;
//                     bmp.data[i + 1] = 255;
//                     bmp.data[i + 2] = 255;
//                 }
//                 else
//                 {
//                     bmp.data[i]     = 0;
//                     bmp.data[i + 1] = 0;
//                     bmp.data[i + 2] = 0;
//                 }
//                 break;
//             case BLUE :
//                 if (bmp.data[i] != 0)
//                 {
//                     bmp.data[i]     = 255;
//                     bmp.data[i + 1] = 255;
//                     bmp.data[i + 2] = 255;
//                 }
//                 else
//                 {
//                     bmp.data[i]     = 0;
//                     bmp.data[i + 1] = 0;
//                     bmp.data[i + 2] = 0;
//                 }
//                 break;
//         }
//     }
//     return bmp;
// }