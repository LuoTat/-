#include "Mat.h"

// 从文件中读取bmp图像，返回一个Mat
Mat imread(const char* filename);

// 将bmp图像写入文件
void imwrite(const char* filename, Mat src);

// // 复制bmp图像
// bmp_HL bmpCopy(bmp_HL bmp);

// // 释放bmp_HL结构体占用的内存
// void bmpDelete(bmp_HL* bmp);

// // 将24位真彩色图像转换为8位灰度图像
// bmp_HL bmp_24to8_gray(bmp_HL bmp);

// // 将8位灰度图像转换为反色8位灰度图像
// bmp_HL bmp_8_grayto8_gray_invert(bmp_HL bmp);

// // 将24位真彩色图像的RGB通道分离
// bmp_HL bmp_24split_rgb_channel(bmp_HL bmp, COLOR color);