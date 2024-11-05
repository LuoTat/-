#include "bmp.h"

int main()
{
    // 读取bmp文件
    bmp_HL bmp             = bmpRead("rgb.bmp");
    // 将24位真彩色图像转换为8位灰度图像
    bmp_HL bmp_gray        = bmp_24to8_gray(bmp);
    // 将8位灰度图像转换为反色8位灰度图像
    bmp_HL bmp_invert_gray = bmp_8_grayto8_gray_invert(bmp_gray);
    // 将24位真彩色图像通道分离
    bmp_HL bmp_r           = bmp_24split_rgb_channel(bmp, RED);
    bmp_HL bmp_g           = bmp_24split_rgb_channel(bmp, GREEN);
    bmp_HL bmp_b           = bmp_24split_rgb_channel(bmp, BLUE);
    // 将分离后的通道转换为8位灰度图像
    bmp_HL bmp_r_gray      = bmp_24to8_gray(bmp_r);
    bmp_HL bmp_g_gray      = bmp_24to8_gray(bmp_g);
    bmp_HL bmp_b_gray      = bmp_24to8_gray(bmp_b);
    // 写入bmp文件
    bmpWrite("rgb_gray.bmp", bmp_gray);
    bmpWrite("rgb_invert_gray.bmp", bmp_invert_gray);
    bmpWrite("rgb_r.bmp", bmp_r);
    bmpWrite("rgb_g.bmp", bmp_g);
    bmpWrite("rgb_b.bmp", bmp_b);
    bmpWrite("rgb_r_gray.bmp", bmp_r_gray);
    bmpWrite("rgb_g_gray.bmp", bmp_g_gray);
    bmpWrite("rgb_b_gray.bmp", bmp_b_gray);
    // 释放内存
    bmpDelete(&bmp);
    bmpDelete(&bmp_gray);
    bmpDelete(&bmp_invert_gray);
    bmpDelete(&bmp_r);
    bmpDelete(&bmp_g);
    bmpDelete(&bmp_b);
    bmpDelete(&bmp_r_gray);
    bmpDelete(&bmp_g_gray);
    bmpDelete(&bmp_b_gray);
    return 0;
}