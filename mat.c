#include "mat.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Mat createMat(int row, int col, short channels, size_t elemSize1)
{
    Mat mat = {
        .rows      = row,
        .cols      = col,
        .channels  = channels,
        .step      = col * channels * elemSize1,
        .elemSize  = channels * elemSize1,
        .elemSize1 = elemSize1,
        .data      = (unsigned char*)calloc(row, col * channels * elemSize1)};
    return mat;
}

Mat copyMat(const Mat* m)
{
    Mat mat  = *m;
    mat.data = (unsigned char*)malloc(m->rows * m->step);
    return mat;
}

void deleteMat(Mat* m)
{
    free(m->data);
    m->data = NULL;
}

void convertTo(const Mat* src, Mat* dst, double scale, double shift)
{
    *dst = createMat(src->rows, src->cols, src->channels, sizeof(size_t));
    for (int y = 0; y < src->rows; y++)
    {
        for (int x = 0; x < src->cols; x++)
        {
            *(size_t*)PIXEL(*dst, x, y) = (size_t)(*(size_t*)PIXEL(*src, x, y) * scale + shift);
        }
    }
}