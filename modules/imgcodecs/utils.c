#include "precomp.h"
#include "utils.h"

#define SCALE 14
#define cR    (int)(0.299 * (1 << SCALE) + 0.5)
#define cG    (int)(0.587 * (1 << SCALE) + 0.5)
#define cB    ((1 << SCALE) - cR - cG)

int validateToInt(size_t sz)
{
    int valueInt = (int)sz;
    assert((size_t)valueInt == sz);
    return valueInt;
}

void ihlCvt_BGR2Gray_8u_C3C1R(const unsigned char* bgr, int bgr_step, unsigned char* gray, int gray_step, int width, int height)
{
    for (; height--; gray += gray_step)
    {
        for (int i = 0; i < width; i++, bgr += 3)
        {
            int t   = descale(bgr[0] * (short)cB + bgr[1] * cG + bgr[2] * (short)cR, SCALE);
            gray[i] = (unsigned char)t;
        }
        bgr += bgr_step - width * 3;
    }
}

void ihlCvt_BGRA2Gray_8u_C4C1R(const unsigned char* bgra, int rgba_step, unsigned char* gray, int gray_step, int width, int height)
{
    for (; height--; gray += gray_step)
    {
        for (int i = 0; i < width; i++, bgra += 4)
        {
            int t   = descale(bgra[0] * (short)cB + bgra[1] * cG + bgra[2] * (short)cR, SCALE);
            gray[i] = (unsigned char)t;
        }

        bgra += rgba_step - width * 4;
    }
}

void ihlCvt_BGRA2BGR_8u_C4C3R(const unsigned char* bgra, int bgra_step, unsigned char* bgr, int bgr_step, int width, int height)
{
    for (; height--;)
    {
        for (int i = 0; i < width; i++, bgr += 3, bgra += 4)
        {
            unsigned char t0 = bgra[0], t1 = bgra[1];
            bgr[0] = t0;
            bgr[1] = t1;
            t0     = bgra[2];
            bgr[2] = t0;
        }
        bgr  += bgr_step - width * 3;
        bgra += bgra_step - width * 4;
    }
}

void ihlCvt_BGR5552Gray_8u_C2C1R(const unsigned char* bgr555, int bgr555_step, unsigned char* gray, int gray_step, int width, int height)
{
    for (; height--; gray += gray_step, bgr555 += bgr555_step)
    {
        for (int i = 0; i < width; i++)
        {
            int t   = descale(((((unsigned short*)bgr555)[i] << 3) & 0xf8) * cB + ((((unsigned short*)bgr555)[i] >> 2) & 0xf8) * cG + ((((unsigned short*)bgr555)[i] >> 7) & 0xf8) * cR, SCALE);
            gray[i] = (unsigned char)t;
        }
    }
}

void ihlCvt_BGR5652Gray_8u_C2C1R(const unsigned char* bgr565, int bgr565_step, unsigned char* gray, int gray_step, int width, int height)
{
    int i;
    for (; height--; gray += gray_step, bgr565 += bgr565_step)
    {
        for (i = 0; i < width; i++)
        {
            int t   = descale(((((unsigned short*)bgr565)[i] << 3) & 0xf8) * cB + ((((unsigned short*)bgr565)[i] >> 3) & 0xfc) * cG + ((((unsigned short*)bgr565)[i] >> 8) & 0xf8) * cR, SCALE);
            gray[i] = (unsigned char)t;
        }
    }
}

void ihlCvt_BGR5552BGR_8u_C2C3R(const unsigned char* bgr555, int bgr555_step, unsigned char* bgr, int bgr_step, int width, int height)
{
    for (; height--; bgr555 += bgr555_step)
    {
        for (int i = 0; i < width; i++, bgr += 3)
        {
            int t0 = (((unsigned short*)bgr555)[i] << 3) & 0xf8;
            int t1 = (((unsigned short*)bgr555)[i] >> 2) & 0xf8;
            int t2 = (((unsigned short*)bgr555)[i] >> 7) & 0xf8;
            bgr[0] = (unsigned char)t0;
            bgr[1] = (unsigned char)t1;
            bgr[2] = (unsigned char)t2;
        }
        bgr += bgr_step - width * 3;
    }
}

void ihlCvt_BGR5652BGR_8u_C2C3R(const unsigned char* bgr565, int bgr565_step, unsigned char* bgr, int bgr_step, int width, int height)
{
    int i;
    for (; height--; bgr565 += bgr565_step)
    {
        for (i = 0; i < width; i++, bgr += 3)
        {
            int t0 = (((unsigned short*)bgr565)[i] << 3) & 0xf8;
            int t1 = (((unsigned short*)bgr565)[i] >> 3) & 0xfc;
            int t2 = (((unsigned short*)bgr565)[i] >> 8) & 0xf8;
            bgr[0] = (unsigned char)t0;
            bgr[1] = (unsigned char)t1;
            bgr[2] = (unsigned char)t2;
        }
        bgr += bgr_step - width * 3;
    }
}

void FillGrayPalette(PaletteEntry* palette, int bpp, bool negative)
{
    int i, length = 1 << bpp;
    int xor_mask = negative ? 255 : 0;

    for (i = 0; i < length; i++)
    {
        int val      = (i * 255 / (length - 1)) ^ xor_mask;
        palette[i].b = palette[i].g = palette[i].r = (unsigned char)val;
        palette[i].a                               = 0;
    }
}

bool IsColorPalette(PaletteEntry* palette, int bpp)
{
    int i, length = 1 << bpp;

    for (i = 0; i < length; i++)
    {
        if (palette[i].b != palette[i].g || palette[i].b != palette[i].r) return true;
    }
    return false;
}

void CvtPaletteToGray(const PaletteEntry* palette, unsigned char* grayPalette, int entries)
{
    for (int i = 0; i < entries; ++i)
    {
        ihlCvt_BGR2Gray_8u_C3C1R((unsigned char*)(palette + i), 0, grayPalette + i, 0, 1, 1);
    }
}

unsigned char* FillUniColor(unsigned char* data, unsigned char** line_end, int step, int width3, int* y, int height, int count3, PaletteEntry clr)
{
    do
    {
        unsigned char* end = data + count3;

        if (end > *line_end)
            end = *line_end;

        count3 -= (int)(end - data);

        for (; data < end; data += 3)
        {
            WRITE_PIX(data, clr);
        }

        if (data >= *line_end)
        {
            *line_end += step;
            data       = *line_end - width3;
            if (++*y >= height) break;
        }
    }
    while (count3 > 0);

    return data;
}

unsigned char* FillUniGray(unsigned char* data, unsigned char** line_end, int step, int width, int* y, int height, int count, unsigned char clr)
{
    do
    {
        unsigned char* end = data + count;

        if (end > *line_end)
            end = *line_end;

        count -= (int)(end - data);

        for (; data < end; data++)
        {
            *data = clr;
        }

        if (data >= *line_end)
        {
            *line_end += step;
            data       = *line_end - width;
            if (++*y >= height) break;
        }
    }
    while (count > 0);

    return data;
}

unsigned char* FillColorRow8(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette)
{
    unsigned char* end = data + len * 3;
    while ((data += 3) < end)
    {
        *((PaletteEntry*)(data - 3)) = palette[*indices++];
    }
    PaletteEntry clr = palette[indices[0]];
    WRITE_PIX(data - 3, clr);
    return data;
}

unsigned char* FillGrayRow8(unsigned char* data, unsigned char* indices, int len, unsigned char* palette)
{
    int i;
    for (i = 0; i < len; i++)
    {
        data[i] = palette[indices[i]];
    }
    return data + len;
}

unsigned char* FillColorRow4(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette)
{
    unsigned char* end = data + len * 3;

    while ((data += 6) < end)
    {
        int idx                      = *indices++;
        *((PaletteEntry*)(data - 6)) = palette[idx >> 4];
        *((PaletteEntry*)(data - 3)) = palette[idx & 15];
    }

    int          idx = indices[0];
    PaletteEntry clr = palette[idx >> 4];
    WRITE_PIX(data - 6, clr);

    if (data == end)
    {
        clr = palette[idx & 15];
        WRITE_PIX(data - 3, clr);
    }
    return end;
}

unsigned char* FillGrayRow4(unsigned char* data, unsigned char* indices, int len, unsigned char* palette)
{
    unsigned char* end = data + len;
    while ((data += 2) < end)
    {
        int idx  = *indices++;
        data[-2] = palette[idx >> 4];
        data[-1] = palette[idx & 15];
    }

    int           idx = indices[0];
    unsigned char clr = palette[idx >> 4];
    data[-2]          = clr;

    if (data == end)
    {
        clr      = palette[idx & 15];
        data[-1] = clr;
    }
    return end;
}

unsigned char* FillColorRow1(unsigned char* data, unsigned char* indices, int len, PaletteEntry* palette)
{
    unsigned char* end    = data + len * 3;

    const PaletteEntry p0 = palette[0], p1 = palette[1];

    while ((data += 24) < end)
    {
        int idx                       = *indices++;
        *((PaletteEntry*)(data - 24)) = (idx & 128) ? p1 : p0;
        *((PaletteEntry*)(data - 21)) = (idx & 64) ? p1 : p0;
        *((PaletteEntry*)(data - 18)) = (idx & 32) ? p1 : p0;
        *((PaletteEntry*)(data - 15)) = (idx & 16) ? p1 : p0;
        *((PaletteEntry*)(data - 12)) = (idx & 8) ? p1 : p0;
        *((PaletteEntry*)(data - 9))  = (idx & 4) ? p1 : p0;
        *((PaletteEntry*)(data - 6))  = (idx & 2) ? p1 : p0;
        *((PaletteEntry*)(data - 3))  = (idx & 1) ? p1 : p0;
    }

    int idx = indices[0];
    for (data -= 24; data < end; data += 3, idx += idx)
    {
        const PaletteEntry clr = (idx & 128) ? p1 : p0;
        WRITE_PIX(data, clr);
    }

    return data;
}

unsigned char* FillGrayRow1(unsigned char* data, unsigned char* indices, int len, unsigned char* palette)
{
    unsigned char* end     = data + len;

    const unsigned char p0 = palette[0], p1 = palette[1];

    while ((data += 8) < end)
    {
        int idx                       = *indices++;
        *((unsigned char*)(data - 8)) = (idx & 128) ? p1 : p0;
        *((unsigned char*)(data - 7)) = (idx & 64) ? p1 : p0;
        *((unsigned char*)(data - 6)) = (idx & 32) ? p1 : p0;
        *((unsigned char*)(data - 5)) = (idx & 16) ? p1 : p0;
        *((unsigned char*)(data - 4)) = (idx & 8) ? p1 : p0;
        *((unsigned char*)(data - 3)) = (idx & 4) ? p1 : p0;
        *((unsigned char*)(data - 2)) = (idx & 2) ? p1 : p0;
        *((unsigned char*)(data - 1)) = (idx & 1) ? p1 : p0;
    }

    int idx = indices[0];
    for (data -= 8; data < end; data++, idx += idx)
    {
        data[0] = (idx & 128) ? p1 : p0;
    }

    return data;
}