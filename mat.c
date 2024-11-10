#include "mat.h"
#include <stdlib.h>
#include <string.h>

Mat createMat(int row, int col, unsigned char channels, size_t elemSize1)
{
    Mat mat;
    mat.rows      = row;
    mat.cols      = col;
    mat.channels  = channels;
    mat.step      = col * channels * elemSize1;
    mat.elemSize  = channels * elemSize1;
    mat.elemSize1 = elemSize1;
    mat.data      = (unsigned char*)malloc(row * mat.step);
    return mat;
}

Mat copyMat(const Mat* m)
{
    Mat mat = createMat(m->rows, m->cols, m->channels, m->elemSize1);
    memcpy(mat.data, m->data, m->rows * m->step);
    return mat;
}

void deleteMat(Mat* m)
{
    free(m->data);
    m->data = NULL;
}

static void cvtBGR2GRAY(const Mat* src, Mat* dst)
{
    // 先创建一个Mat对象
    *dst = createMat(src->rows, src->cols, 1, sizeof(unsigned char));
    // 遍历每个像素点
    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            unsigned char* src_pixel = PIXEL(*src, i, j);
            unsigned char* dst_pixel = PIXEL(*dst, i, j);
            *dst_pixel               = (unsigned char)(0.114 * src_pixel[0] + 0.587 * src_pixel[1] + 0.299 * src_pixel[2]);
        }
    }
}

static void cvtBGR2GRAY_INVERTED(const Mat* src, Mat* dst)
{
    // 先创建一个Mat对象
    *dst = createMat(src->rows, src->cols, 1, sizeof(unsigned char));
    // 遍历每个像素点
    for (int i = 0; i < src->rows; i++)
    {
        for (int j = 0; j < src->cols; j++)
        {
            unsigned char* src_pixel = PIXEL(*src, i, j);
            unsigned char* dst_pixel = PIXEL(*dst, i, j);
            *dst_pixel               = (unsigned char)(255 - (0.114 * src_pixel[0] + 0.587 * src_pixel[1] + 0.299 * src_pixel[2]));
        }
    }
}

void cvtColor(const Mat* src, Mat* dst, enum ColorConversionCodes code)
{
    switch (code)
    {
        case COLOR_BGR2GRAY :
            cvtBGR2GRAY(src, dst);
            break;
        case COLOR_BGR2GRAY_INVERTED :
            cvtBGR2GRAY_INVERTED(src, dst);
            break;
        default :
            break;
    }
}

void split(const Mat* m, Mat* mv)
{
    // 获取通道数
    unsigned char channels = m->channels;

    // 创建channels个Mat对象
    for (int i = 0; i < channels; ++i)
    {
        mv[i] = createMat(m->rows, m->cols, 1, sizeof(unsigned char));
    }
    // 遍历每个像素点
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            unsigned char* m_pixel = PIXEL(*m, i, j);
            for (int k = 0; k < channels; k++)
            {
                unsigned char* mv_pixel = PIXEL(mv[k], i, j);
                *mv_pixel               = m_pixel[k];
            }
        }
    }
}