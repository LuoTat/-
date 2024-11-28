#include "precomp.h"
#include <stdlib.h>

// This function blurs the input image using a box filter.
void blurs(const Mat* src, Mat* dst, int ksize)
{
    int border = ksize / 2;

    // 创建目标图像
    create(dst, src->rows, src->cols, type(src));

    // 遍历图像的每个像素点
    for (int y = 0; y < src->rows; y++)
    {
        for (int x = 0; x < src->cols; x++)
        {
            int sum   = 0;
            int count = 0;

            // 计算邻域内的像素和
            for (int i = -border; i <= border; i++)
            {
                for (int j = -border; j <= border; j++)
                {
                    int ny = y + i;
                    int nx = x + j;

                    // 边界处理，复制边界
                    if (ny < 0) ny = 0;
                    if (ny >= src->rows) ny = src->rows - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= src->cols) nx = src->cols - 1;

                    sum += *PIXEL(*src, unsigned char, nx, ny);
                    count++;
                }
            }

            // 计算平均值并赋值给目标图像
            *PIXEL(*dst, unsigned char, x, y) = sum / count;
        }
    }
}

// Helper function to compare two integers (used for qsort)
int compare(const void* a, const void* b)
{
    return (*(unsigned char*)a - *(unsigned char*)b);
}

// This function applies median filter to the input image.
void medianBlur(const Mat* src, Mat* dst, int ksize)
{
    int border = ksize / 2;

    // 创建目标图像
    create(dst, src->rows, src->cols, type(src));

    // 临时数组用于存储邻域像素值
    unsigned char* neighborhood = (unsigned char*)malloc(ksize * ksize * sizeof(unsigned char));

    // 遍历图像的每个像素点
    for (int y = 0; y < src->rows; y++)
    {
        for (int x = 0; x < src->cols; x++)
        {
            int count = 0;

            // 收集邻域内的像素值
            for (int i = -border; i <= border; i++)
            {
                for (int j = -border; j <= border; j++)
                {
                    int ny = y + i;
                    int nx = x + j;

                    // 边界处理，复制边界
                    if (ny < 0) ny = 0;
                    if (ny >= src->rows) ny = src->rows - 1;
                    if (nx < 0) nx = 0;
                    if (nx >= src->cols) nx = src->cols - 1;

                    neighborhood[count++] = *PIXEL(*src, unsigned char, nx, ny);
                }
            }

            // 对邻域像素值进行排序
            qsort(neighborhood, count, sizeof(unsigned char), compare);

            // 取中值并赋值给目标图像
            *PIXEL(*dst, unsigned char, x, y) = neighborhood[count / 2];
        }
    }

    // 释放临时数组
    free(neighborhood);
}