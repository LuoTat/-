#include "Mat.h"
#include <stdlib.h>

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

void cvtColor(Mat src, Mat* dst, enum ColorConversionCodes code)
{
    switch (code)
    {
        case COLOR_BGR2GRAY :
            // 先创建一个Mat对象
            *dst = createMat(src.rows, src.cols, 1, 1);
            // 遍历每个像素点
            for (int i = 0; i < src.rows; i++)
            {
                for (int j = 0; j < src.cols; j++)
                {
                    unsigned char* src_pixel = PIXEL(src, i, j);
                    unsigned char* dst_pixel = PIXEL((*dst), i, j);
                    *dst_pixel               = (unsigned char)(0.114 * src_pixel[0] + 0.587 * src_pixel[1] + 0.299 * src_pixel[2]);
                }
            }
            break;
        default :
            break;
    }
}