#pragma once

#include <stddef.h>

// 定位Mat中的像素位置
#define PIXEL(Mat, x, y) ((Mat).data + (y) * (Mat).step + (x) * (Mat).elemSize)

typedef struct
{
    int            rows;         // 行数
    int            cols;         // 列数
    short          channels;     // 通道数
    size_t         step;         // 每行的字节数
    size_t         elemSize;     // 每个元素的的字节数
    size_t         elemSize1;    // 每个元素中一个通道的的字节数
    unsigned char* data;         // 数据
} Mat;

// 创建一个Mat对象
Mat createMat(int row, int col, short channels, size_t elemSize1);

// 深拷贝
Mat copyMat(const Mat* m);

// 销毁Mat对象
void deleteMat(Mat* m);