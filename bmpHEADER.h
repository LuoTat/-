#include <stdint.h>
#define WORD uint16_t
#define DWORD uint32_t
#define LONG int32_t
#define BYTE uint8_t

//定义位图文件头
# pragma pack(push, 2) //强制按2字节对齐
typedef struct tagBITMAPFILEHEADER{
    WORD   	bfType; //文件类型，必须是“BM”
    DWORD	bfSize; //文件大小，包含文件头的大小
    WORD	bfReserved1; //保留字
    WORD	bfReserved2; //保留字
    DWORD	bfOffBits; //从文件头到实际位图数据的偏移字节数
} BITMAPFILEHEADER; 
# pragma pack(pop)

//定义位图信息头
typedef struct tagBITMAPINFOHEADER{
    DWORD   biSize; //该结构的长度，为40
    LONG	biWidth; //图像宽度
    LONG	biHeight; //图像高度
    WORD	biPlanes; //位平面数，必须为1
    WORD    biBitCount; //颜色位数，
    DWORD   biCompression; //是否压缩
    DWORD   biSizeImage; //实际位图数据占用的字节数
    LONG	biXPelsPerMeter;//目标设备水平分辨率
    LONG	biYPelsPerMeter;//目标设备垂直分辨率
    DWORD   biClrUsed;//实际使用的颜色数
    DWORD   biClrImportant;//图像中重要的颜色数
} BITMAPINFOHEADER; 

//定义调色板
typedef struct tagRGBQUAD{
    BYTE    rgbBlue; //蓝色分量
    BYTE    rgbGreen; //绿色分量
    BYTE    rgbRed; //红色分量
    BYTE    rgbReserved; //保留字
} RGBQUAD;

typedef RGBQUAD* PALETTE;