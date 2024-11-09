#include "bmp.h"
#include "bmp_core.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Mat imread(const char* filename, enum ImreadModes mode)
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER infoHeader;

    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    // 读取文件头和信息头
    fread(&header, sizeof(BITMAPFILEHEADER), 1, file);
    fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    if (header.bfType != 0x4D42)
    {
        printf("Error: Not a BMP file\n");
        exit(1);
    }

    int            rows     = infoHeader.biHeight;
    int            cols     = infoHeader.biWidth;
    unsigned short channels = infoHeader.biBitCount / 8;
    unsigned int   step     = LineByte(cols, infoHeader.biBitCount);

    // 直接跳转到数据区
    fseek(file, header.bfOffBits, SEEK_SET);
    // 读取图像数据
    unsigned char* data = (unsigned char*)malloc(infoHeader.biSizeImage);
    fread(data, infoHeader.biSizeImage, 1, file);
    fclose(file);

    Mat mat;

    switch (mode)
    {
        case IMREAD_UNCHANGED :    // 读取原始图像
            mat = createMat(rows, cols, channels, 1);
            for (int i = 0; i < infoHeader.biHeight; ++i)
            {
                memcpy(mat.data + i * mat.step, data + i * step, mat.step);
            }
            break;
        case IMREAD_COLOR :    // 读取BGR3通道图像
            mat = createMat(rows, cols, 3, 1);
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    unsigned char* mat_pixel = PIXEL(mat, i, j);
                    switch (infoHeader.biBitCount)
                    {
                        case 32 :    // 舍去alpha通道
                            mat_pixel[0] = data[i * step + j * 4 + 0];
                            mat_pixel[1] = data[i * step + j * 4 + 1];
                            mat_pixel[2] = data[i * step + j * 4 + 2];
                            break;
                        case 24 :
                            mat_pixel[0] = data[i * step + j * 3 + 0];
                            mat_pixel[1] = data[i * step + j * 3 + 1];
                            mat_pixel[2] = data[i * step + j * 3 + 2];
                            break;
                        case 8 :    // 伪彩色图像
                            mat_pixel[0] = data[i * step + j];
                            mat_pixel[1] = data[i * step + j];
                            mat_pixel[2] = data[i * step + j];
                            break;
                        default :
                            printf("Error: Unsupported bit count %d\n", infoHeader.biBitCount);
                            exit(1);
                    }
                }
            }
            break;
        case IMREAD_GRAYSCALE :    // 读取8位灰度图像
            mat = createMat(rows, cols, 1, 1);
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    unsigned char* mat_pixel = PIXEL(mat, i, j);
                    switch (infoHeader.biBitCount)
                    {
                        case 32 :
                            *mat_pixel = (unsigned char)(0.114 * data[i * step + j * 4 + 0] + 0.587 * data[i * step + j * 4 + 1] + 0.299 * data[i * step + j * 4 + 2]);
                            break;
                        case 24 :
                            *mat_pixel = (unsigned char)(0.114 * data[i * step + j * 3 + 0] + 0.587 * data[i * step + j * 3 + 1] + 0.299 * data[i * step + j * 3 + 2]);
                            break;
                        case 8 :
                            *mat_pixel = data[i * step + j];
                            break;
                        default :
                            printf("Error: Unsupported bit count %d\n", infoHeader.biBitCount);
                            exit(1);
                    }
                }
            }
    }

    free(data);
    return mat;
}

void imwrite(const char* filename, Mat src)
{
    BITMAPFILEHEADER header;
    BITMAPINFOHEADER infoHeader;
    unsigned int     step      = LineByte(src.cols, src.elemSize * CHAR_BIT);

    // 填充文件头
    header.bfType              = 0x4D42;
    header.bfSize              = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + src.rows * src.step;
    header.bfReserved1         = 0;
    header.bfReserved2         = 0;
    header.bfOffBits           = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // 填充信息头
    infoHeader.biSize          = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth         = src.cols;
    infoHeader.biHeight        = src.rows;
    infoHeader.biPlanes        = 1;
    infoHeader.biBitCount      = src.channels * 8;
    infoHeader.biCompression   = 0;
    infoHeader.biSizeImage     = src.rows * step;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed       = 0;
    infoHeader.biClrImportant  = 0;

    FILE* file                 = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }
    fwrite(&header, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    for (int i = 0; i < src.rows; ++i)
    {
        fwrite(src.data + i * src.step, step, 1, file);
    }
    fclose(file);
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