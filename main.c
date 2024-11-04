#include "bmp.h"

int main()
{
    // 读取bmp文件
    bmp_HL bmp             = bmpRead("Data.bmp");
    // 将24位真彩色图像转换为8位灰度图像
    bmp_HL bmp_gray        = bmp_24to8_gray(bmp);
    // 将8位灰度图像转换为反色8位灰度图像
    bmp_HL bmp_invert_gray = bmp_8_graytoinvert_gray(bmp_gray);
    // 写入bmp文件
    bmpWrite("Data_gray.bmp", bmp_gray);
    bmpWrite("Data_invert_gray.bmp", bmp_invert_gray);
    // 释放内存
    bmpDelete(&bmp);
    bmpDelete(&bmp_gray);
    bmpDelete(&bmp_invert_gray);
    return 0;
}