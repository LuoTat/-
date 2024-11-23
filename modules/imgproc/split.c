#include "mat.h"

void split(const Mat* src, Mat* mvbegin)
{
    int channels = src->channels;
    if (channels == 1)
    {
        mvbegin[0] = copyMat(src);
    }

    for (int i = 0; i < channels; ++i)
    {
        mvbegin[i] = createMat(src->rows, src->cols, 1, sizeof(unsigned char));
    }

    for (int y = 0; y < src->rows; ++y)
    {
        for (int x = 0; x < src->cols; ++x)
        {
            unsigned char* src_pixel = PIXEL(*src, x, y);
            for (int i = 0; i < channels; ++i)
            {
                unsigned char* mv_pixel = PIXEL(mvbegin[i], x, y);
                *mv_pixel               = src_pixel[i];
            }
        }
    }
}