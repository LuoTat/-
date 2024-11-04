#include "bmpHEADER.h"

typedef struct
{
    bmpHEADER      header;
    bmpINFOHEADER  infoHeader;
    PALETTE        palette;
    unsigned char* data;
} bmp_HL;

// 从文件中读取bmp图像
bmp_HL bmpRead(const char* filename);