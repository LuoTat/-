#include <stddef.h>

// 用来定位Mat中的像素位置
#define PIXEL(Mat, i, j) ((Mat).data + (i) * (Mat).step + (j) * (Mat).elemSize)

typedef struct
{
    unsigned int   rows;         // 行数
    unsigned int   cols;         // 列数
    unsigned char  channels;     // 通道数
    size_t         step;         // 每行的字节数
    size_t         elemSize;     // 每个元素的大小
    size_t         elemSize1;    // 每个元素中一个通道的大小
    unsigned char* data;         // 数据
} Mat;

enum ColorConversionCodes
{
    COLOR_BGR2GRAY,
    COLOR_BGR2GRAY_INVERTED
};

// 创建一个Mat对象
Mat createMat(int row, int col, unsigned char channels, size_t elemSize1);

// 颜色空间转换
void cvtColor(Mat src, Mat* dst, enum ColorConversionCodes code);

// 分离通道
void split(Mat src, Mat* dst);