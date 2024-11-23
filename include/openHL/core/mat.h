#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/base.h"
#include <stdlib.h>

// 定位Mat中的像素位置
#define PIXEL(Mat, TYPE, x, y) ((TYPE*)((Mat).data + (Mat).step[0] * (y) + (Mat).step[1] * (x)))

typedef struct
{
    int            flags;      // 标志位
    int            rows;       // 行数
    int            cols;       // 列数
    size_t         step[2];    // 步长
    unsigned char* data;       // 数据
} Mat;

// 创建一个Mat对象
void create(Mat* m, int row, int col, int type);

// 深拷贝
void copyTo(const Mat* m, Mat* out);

// 销毁Mat对象
void release(Mat* m);

// 获得元素大小
size_t elemSize(const Mat* m);

// 获得单个通道元素大小
size_t elemSize1(const Mat* m);

// 获得type
int type(const Mat* m);

// 获得depth
int depth(const Mat* m);

// 获得通道数
int channels(const Mat* m);

// 获得一行的通道数
size_t step1(const Mat* m, int i);

// 判断是否为空
bool empty(const Mat* m);

// 获得总元素数
size_t total(const Mat* m);

// 将src放缩到dst
void convertTo(const Mat* src, Mat* dst, int rtype, double alpha, double beta);