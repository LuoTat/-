#pragma pack(push, 2)

typedef struct
{
    unsigned short bfType __attribute__((packed));         // 文件类型
    unsigned int   bfSize;                                 // 文件大小（包括文件头）
    unsigned short bfReserved1 __attribute__((packed));    // 保留字1
    unsigned short bfReserved2 __attribute__((packed));    // 保留字2
    unsigned int   bfOffBits;                              // 从文件头到实际位图数据的偏移字节数
} bmpHEADER;

#pragma pack(pop)

typedef struct
{
    unsigned int   biSize;             // 信息头大小
    int            biWidth;            // 图像宽度
    int            biHeight;           // 图像高度
    unsigned short biPlanes;           // 位平面数，必须为1
    unsigned short biBitCount;         // 每个像素的位数
    unsigned int   biCompression;      // 压缩类型
    unsigned int   biSizeImage;        // 图像大小
    int            biXPelsPerMeter;    // 水平分辨率
    int            biYPelsPerMeter;    // 垂直分辨率
    unsigned int   biClrUsed;          // 使用的颜色数
    unsigned int   biClrImportant;     // 重要的颜色数
} bmpINFOHEADER;

typedef struct
{
    unsigned char rgbBlue;             // 蓝色分量
    unsigned char rgbGreen;            // 绿色分量
    unsigned char rgbRed;              // 红色分量
    unsigned char rgbReserved;         // 保留字
} RGBQUAD;

typedef RGBQUAD* PALETTE;