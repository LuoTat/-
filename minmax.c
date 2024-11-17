#include "mat.h"
#include <stdio.h>

void minMaxIdx(const Mat* src, double* minVal, double* maxVal, int* minIdx, int* maxIdx)
{
    if (src->channels != 1)
    {
        printf("channels must be 1\n");
        return;
    }

    *minVal = *maxVal = (double)(*(size_t*)PIXEL(*src, 0, 0));
    int minIdx_x = 0, minIdx_y = 0, maxIdx_x = 0, maxIdx_y = 0;
    for (int y = 0; y < src->rows; ++y)
    {
        for (int x = 0; x < src->cols; ++x)
        {
            double val = (double)*(size_t*)PIXEL(*src, x, y);
            if (val < *minVal)
            {
                *minVal  = val;
                minIdx_x = x;
                minIdx_y = y;
            }
            if (val > *maxVal)
            {
                *maxVal  = val;
                maxIdx_x = x;
                maxIdx_y = y;
            }
        }
    }
    if (minIdx != NULL)
    {
        minIdx[0] = minIdx_x;
        minIdx[1] = minIdx_y;
    }
    if (maxIdx != NULL)
    {
        maxIdx[0] = maxIdx_x;
        maxIdx[1] = maxIdx_y;
    }
}