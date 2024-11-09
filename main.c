#include "bmp.h"

int main()
{
    // 读取bmp文件
    Mat rgb = imread("rgb.bmp");
    // 将24位真彩色图像转换为8位灰度图像
    Mat gray;
    cvtColor(rgb, &gray, COLOR_BGR2GRAY);
    // 将24位真彩色图像转换为8位灰度图像并反转
    Mat gray_inverted;
    cvtColor(rgb, &gray_inverted, COLOR_BGR2GRAY_INVERTED);
    // 通道分离
    Mat out_array[3];
    split(rgb, out_array);
    // 写入bmp文件
    imwrite("rgb_gray.bmp", gray);
    imwrite("rgb_gray_inverted.bmp", gray_inverted);
    imwrite("rgb_blue.bmp", out_array[0]);
    imwrite("rgb_green.bmp", out_array[1]);
    imwrite("rgb_red.bmp", out_array[2]);
    return 0;
}