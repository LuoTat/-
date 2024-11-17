#include "core.h"
#include "mat.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void calcHist(const Mat* images, Mat* hist)
{
    if (images->channels != 1)
    {
        printf("Error: calcHist() only works with single channel images\n");
        return;
    }

    // 创建一个256*1的灰度矩阵，元素类型为size_t
    *hist = createMat(1, 256, 1, sizeof(size_t));

    // 遍历所有的像素值
    for (int y = 0; y < images->rows; ++y)
    {
        for (int x = 0; x < images->cols; ++x)
        {
            ++*(size_t*)PIXEL(*hist, *PIXEL(*images, x, y), 0);
        }
    }
}

void drawHist(const Mat* hist, Mat* histImage, int width, int height)
{
    *histImage = createMat(height, 256 * width, 1, sizeof(unsigned char));

    for (int y = 0; y < histImage->rows; ++y)
    {
        for (int x = 0; x < histImage->cols; x += width)
        {
            size_t* hist_pixi = (size_t*)PIXEL(*hist, x / width, 0);
            if (*hist_pixi == 0)
            {
                memset(PIXEL(*histImage, x, y), 255, width);
            }
            else
            {
                memset(PIXEL(*histImage, x, y), 0, width);
                --*hist_pixi;
            }
        }
    }
}