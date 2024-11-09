#include "bmp.h"

int main()
{
    // 读取bmp文件
    Mat mat = imread("rgb.bmp", IMREAD_COLOR);
    // // 将24位真彩色图像转换为8位灰度图像
    Mat gray;
    cvtColor(mat, &gray, COLOR_BGR2GRAY);
    // // 写入bmp文件
    imwrite("rgb_gray.bmp", gray);
    return 0;
}