#include "mat.h"
#include <stddef.h>
#include <stdio.h>

void calcHist(const Mat* images, Mat* hist)
{
    if (images->channels != 1)
    {
        printf("Error: calcHist() only works with single channel images\n");
        return;
    }

    // 创建一个1000*256+10的矩阵(左右各留5个像素的空白)
    *hist             = createMat(1000, 256 + 10, 1, sizeof(unsigned char));
    // 用于存储每个像素值的个数
    size_t count[256] = {0};

    // 遍历所有的像素值
    for (int y = 0; y < images->rows; ++y)
    {
        for (int x = 0; x < images->cols; ++x)
        {
            unsigned char* pixel = PIXEL(*images, x, y);
            ++count[*pixel];
        }
    }

    // 将count归一化到0-1000
    size_t max = 0;
    for (int i = 0; i < 256; ++i)
    {
        if (count[i] > max)
        {
            max = count[i];
        }
    }
    for (int i = 0; i < 256; ++i)
    {
        count[i] = count[i] * 1000 / max;
    }

    // 将count的值赋值给hist
    for (int y = 0; y < hist->rows; ++y)
    {
        for (int x = 0; x < hist->cols; ++x)
        {
            unsigned char* pixel = PIXEL(*hist, x, y);
            if (x < 5 || x > 260)    // 左右的空白部分
            {
                *pixel = 255;
            }
            else if (count[x - 5] == 0)
            {
                *pixel = 255;
            }
            else
            {
                *pixel = 0;
                --count[x - 5];
            }
        }
    }
}