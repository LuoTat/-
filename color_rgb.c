#include "color.h"

void cvtColorBGR2Gray(const Mat* src, Mat* dst)
{
    // 先创建一个Mat对象
    *dst = createMat(src->rows, src->cols, 1, sizeof(unsigned char));
    // 遍历每个像素点
    for (int y = 0; y < src->rows; ++y)
    {
        for (int x = 0; x < src->cols; ++x)
        {
            unsigned char* src_pixel = PIXEL(*src, x, y);
            unsigned char* dst_pixel = PIXEL(*dst, x, y);
            *dst_pixel               = (unsigned char)(0.114 * src_pixel[0] + 0.587 * src_pixel[1] + 0.299 * src_pixel[2]);
        }
    }
}

void cvtColorBGR2Gray_Inverted(const Mat* src, Mat* dst)
{
    // 先创建一个Mat对象
    *dst = createMat(src->rows, src->cols, 1, sizeof(unsigned char));
    // 遍历每个像素点
    for (int y = 0; y < src->rows; ++y)
    {
        for (int x = 0; x < src->cols; ++x)
        {
            unsigned char* src_pixel = PIXEL(*src, x, y);
            unsigned char* dst_pixel = PIXEL(*dst, x, y);
            *dst_pixel               = (unsigned char)(255 - (0.114 * src_pixel[0] + 0.587 * src_pixel[1] + 0.299 * src_pixel[2]));
        }
    }
}