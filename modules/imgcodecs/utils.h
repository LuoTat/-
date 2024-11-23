#pragma once

#define WRITE_PIX(ptr, clr)                \
    (((unsigned char*)(ptr))[0] = (clr).b, \
     ((unsigned char*)(ptr))[1] = (clr).g, \
     ((unsigned char*)(ptr))[2] = (clr).r)

#define descale(x, n) (((x) + (1 << ((n) - 1))) >> (n))

typedef struct
{
    unsigned char b;    // 蓝色分量
    unsigned char g;    // 绿色分量
    unsigned char r;    // 红色分量
    unsigned char a;    // Alpha分量
} PaletteEntry;

// 将size_t类型转换为int类型
int validateToInt(size_t step);

void ihlCvt_BGR2Gray_8u_C3C1R(const unsigned char* bgr, int bgr_step, unsigned char* gray, int gray_step, int width, int height);
void ihlCvt_BGRA2Gray_8u_C4C1R(const unsigned char* bgra, int rgba_step, unsigned char* gray, int gray_step, int width, int height);
void ihlCvt_BGRA2BGR_8u_C4C3R(const unsigned char* bgra, int bgra_step, unsigned char* bgr, int bgr_step, int width, int height);

void ihlCvt_BGR5552Gray_8u_C2C1R(const unsigned char* bgr555, int bgr555_step, unsigned char* gray, int gray_step, int width, int height);
void ihlCvt_BGR5652Gray_8u_C2C1R(const unsigned char* bgr565, int bgr565_step, unsigned char* gray, int gray_step, int width, int height);
void ihlCvt_BGR5552BGR_8u_C2C3R(const unsigned char* bgr555, int bgr555_step, unsigned char* bgr, int bgr_step, int width, int height);
void ihlCvt_BGR5652BGR_8u_C2C3R(const unsigned char* bgr565, int bgr565_step, unsigned char* bgr, int bgr_step, int width, int height);

void FillGrayPalette(PaletteEntry* palette, int bpp, bool negative);
bool IsColorPalette(PaletteEntry* palette, int bpp);
void CvtPaletteToGray(const PaletteEntry* palette, unsigned char* grayPalette, int entries);

unsigned char* FillUniColor(unsigned char* data, unsigned char** line_end, int step, int width3, int* y, int height, int count3, PaletteEntry clr);
unsigned char* FillUniGray(unsigned char* data, unsigned char** line_end, int step, int width3, int* y, int height, int count3, unsigned char clr);
unsigned char* FillColorRow8(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette);
unsigned char* FillGrayRow8(unsigned char* data, unsigned char* indices, int len, unsigned char* palette);
unsigned char* FillColorRow4(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette);
unsigned char* FillGrayRow4(unsigned char* data, unsigned char* indices, int len, unsigned char* palette);
unsigned char* FillColorRow1(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette);
unsigned char* FillGrayRow1(unsigned char* data, unsigned char* indices, int len, unsigned char* palette);
