#include "precomp.h"
#include "utils.h"
#include "grfmt_bmp.h"

static void initMask(BmpDecoder* decoder)
{
    memset(decoder->rgba_mask, 0, sizeof(decoder->rgba_mask));
    memset(decoder->rgba_bit_offset, -1, sizeof(decoder->rgba_bit_offset));
    for (size_t i = 0; i < 4; i++)
    {
        decoder->rgba_scale_factor[i] = 1.0f;
    }
}

BmpDecoder initBmpDecoder()
{
    BmpDecoder decoder = {
        .height   = 0,
        .width    = 0,
        .type     = -1,
        .filename = NULL,
        .file     = NULL,
        .offset   = -1,
        .bpp      = 0,
        .rle_code = BMP_RGB,
    };
    initMask(&decoder);
    return decoder;
}

void setSource(BmpDecoder* decoder, const char* filename)
{
    decoder->filename = filename;
}

bool readHeader(BmpDecoder* decoder)
{
    bool result  = false;
    bool iscolor = false;

    if (!(decoder->file = fopen(decoder->filename, "rb")))
        return false;

    fseek(decoder->file, 10, SEEK_CUR);                            // 跳过bfType，bfSize，bfReserved1，bfReserved2
    fread(&decoder->offset, sizeof(int), 1, decoder->file);        // 读取bfOffBits

    int size;
    fread(&size, sizeof(int), 1, decoder->file);                   // 读取biSize
    assert(size > 0);

    initMask(decoder);
    if (size >= 36)
    {
        fread(&decoder->width, sizeof(int), 1, decoder->file);     // 读取biWidth
        fread(&decoder->height, sizeof(int), 1, decoder->file);    // 读取biHeight
        fseek(decoder->file, 2, SEEK_CUR);                         // 跳过biPlanes
        fread(&decoder->bpp, sizeof(short), 1, decoder->file);     // 读取biBitCount
        int rle_code_;
        fread(&rle_code_, sizeof(int), 1, decoder->file);          // 读取biCompression
        assert(rle_code_ >= 0 && rle_code_ <= BMP_BITFIELDS);
        decoder->rle_code = (BmpCompression)rle_code_;
        fseek(decoder->file, 12, SEEK_CUR);                        // 跳过biSizeImage，biXPelsPerMeter，biYPelsPerMeter
        int clrused;
        fread(&clrused, sizeof(int), 1, decoder->file);            // 读取biClrUsed

        if (decoder->bpp == 32 && decoder->rle_code == BMP_BITFIELDS && size >= 56)
        {
            fseek(decoder->file, 4, SEEK_CUR);                     // 跳过biClrImportant
            for (int index_rgba = 0; index_rgba < 4; ++index_rgba)
            {
                unsigned int mask;
                fread(&mask, sizeof(int), 1, decoder->file);
                decoder->rgba_mask[index_rgba] = mask;
                if (mask != 0)
                {
                    int bit_count = 0;
                    while (!(mask & 1))
                    {
                        mask >>= 1;
                        ++bit_count;
                    }
                    decoder->rgba_bit_offset[index_rgba]   = bit_count;
                    decoder->rgba_scale_factor[index_rgba] = 255.0f / mask;
                }
            }
            fseek(decoder->file, size - 56, SEEK_CUR);    // 跳过剩下的部分
        }
        else
            fseek(decoder->file, size - 36, SEEK_CUR);    // 跳过剩下的部分

        // 验证头部信息是否合法
        if (decoder->width > 0 && decoder->height != 0 && (((decoder->bpp == 1 || decoder->bpp == 4 || decoder->bpp == 8 || decoder->bpp == 24 || decoder->bpp == 32) && decoder->rle_code == BMP_RGB) || ((decoder->bpp == 16 || decoder->bpp == 32) && (decoder->rle_code == BMP_RGB || decoder->rle_code == BMP_BITFIELDS)) || (decoder->bpp == 4 && decoder->rle_code == BMP_RLE4) || (decoder->bpp == 8 && decoder->rle_code == BMP_RLE8)))
        {
            iscolor = true;
            result  = true;
            if (decoder->bpp <= 8)
            {
                assert(clrused > 0 && clrused <= 256);
                memset(decoder->palette, 0, sizeof(decoder->palette));
                fread(decoder->palette, sizeof(char), (clrused == 0 ? 1 << decoder->bpp : clrused) * 4, decoder->file);
                iscolor = IsColorPalette(decoder->palette, decoder->bpp);
            }
            else if (decoder->bpp == 16 && decoder->rle_code == BMP_BITFIELDS)
            {
                int redmask;
                int greenmask;
                int bluemask;
                fread(&redmask, sizeof(int), 1, decoder->file);
                fread(&greenmask, sizeof(int), 1, decoder->file);
                fread(&bluemask, sizeof(int), 1, decoder->file);
                if (bluemask == 0x1f && greenmask == 0x3e0 && redmask == 0x7c00)
                    decoder->bpp = 15;
                else if (bluemask == 0x1f && greenmask == 0x7e0 && redmask == 0xf800)
                    ;
                else
                    result = false;
            }
            else if (decoder->bpp == 32 && decoder->rle_code == BMP_BITFIELDS)
            {
                // 32bit BMP not require to check something - we can simply allow it to use
                ;
            }
            else if (decoder->bpp == 16 && decoder->rle_code == BMP_RGB)
                decoder->bpp = 15;
        }
    }
    else if (size == 12)
    {
        fread(&decoder->width, sizeof(short), 1, decoder->file);     // 读取biWidth
        fread(&decoder->height, sizeof(short), 1, decoder->file);    // 读取biHeight
        fseek(decoder->file, 2, SEEK_CUR);                           // 跳过biPlanes
        fread(&decoder->bpp, sizeof(short), 1, decoder->file);       // 读取biBitCount
        decoder->rle_code = BMP_RGB;

        if (decoder->width > 0 && decoder->height != 0 && (decoder->bpp == 1 || decoder->bpp == 4 || decoder->bpp == 8 || decoder->bpp == 24 || decoder->bpp == 32))
        {
            if (decoder->bpp <= 8)
            {
                unsigned char buffer[256 * 3];
                int           j, clrused = 1 << decoder->bpp;
                fread(buffer, sizeof(char), clrused * 3, decoder->file);
                for (j = 0; j < clrused; j++)
                {
                    decoder->palette[j].b = buffer[3 * j + 0];
                    decoder->palette[j].g = buffer[3 * j + 1];
                    decoder->palette[j].r = buffer[3 * j + 2];
                }
            }
            result = true;
        }
    }
    decoder->type   = iscolor ? ((decoder->bpp == 32 && decoder->rle_code != BMP_RGB) ? HL_8UC4 : HL_8UC3) : HL_8UC1;
    decoder->height = abs(decoder->height);
    if (!result)
    {
        decoder->offset = -1;
        decoder->width = decoder->height = -1;
        close(decoder);
    }
    return result;
}

static void maskBGRA(const BmpDecoder* decoder, unsigned char* des, const unsigned char* src, int num, bool alpha_required)
{
    int dest_stride = alpha_required ? 4 : 3;
    for (int i = 0; i < num; i++, des += dest_stride, src += 4)
    {
        uint data = *((uint*)src);
        des[0]    = (unsigned char)(((decoder->rgba_mask[2] & data) >> decoder->rgba_bit_offset[2]) * decoder->rgba_scale_factor[2]);
        des[1]    = (unsigned char)(((decoder->rgba_mask[1] & data) >> decoder->rgba_bit_offset[1]) * decoder->rgba_scale_factor[1]);
        des[2]    = (unsigned char)(((decoder->rgba_mask[0] & data) >> decoder->rgba_bit_offset[0]) * decoder->rgba_scale_factor[0]);
        if (alpha_required)
        {
            if (decoder->rgba_bit_offset[3] >= 0)
                des[3] = (unsigned char)(((decoder->rgba_mask[3] & data) >> decoder->rgba_bit_offset[3]) * decoder->rgba_scale_factor[3]);
            else
                des[3] = 255;
        }
    }
}

static void maskBGRAtoGray(const BmpDecoder* decoder, unsigned char* des, const unsigned char* src, int num)
{
    for (int i = 0; i < num; i++, des++, src += 4)
    {
        unsigned int data  = *((unsigned int*)src);
        int          red   = (unsigned char)(((decoder->rgba_mask[0] & data) >> decoder->rgba_bit_offset[0]) * decoder->rgba_scale_factor[0]);
        int          green = (unsigned char)(((decoder->rgba_mask[1] & data) >> decoder->rgba_bit_offset[1]) * decoder->rgba_scale_factor[1]);
        int          blue  = (unsigned char)(((decoder->rgba_mask[2] & data) >> decoder->rgba_bit_offset[2]) * decoder->rgba_scale_factor[2]);
        *des               = (unsigned char)(0.299f * red + 0.587f * green + 0.114f * blue);
    }
}

bool readData(BmpDecoder* decoder, Mat* img)
{
    unsigned char* data              = img->data;
    int            step              = validateToInt(img->step[0]);
    bool           color             = channels(img) > 1;
    unsigned char  gray_palette[256] = {0};
    bool           result            = false;
    int            src_pitch         = ((decoder->width * (decoder->bpp != 15 ? decoder->bpp : 16) + 7) / 8 + 3) & -4;
    int            nch               = color ? 3 : 1;
    int            y, width3 = decoder->width * nch;

    assert((unsigned long)decoder->height * decoder->width * nch < (1ULL << 30));


    if (decoder->offset < 0 || !decoder->file)
        return false;


    // 临时存储从文件读取的数据
    unsigned char* src = (unsigned char*)alloca((src_pitch + 32) * sizeof(char));
    // 临时存储从调色板转换为 BGR 的中间数据
    unsigned char* bgr = (unsigned char*)malloc(1032 * sizeof(char));

    if (!color)
    {
        if (decoder->bpp <= 8)
        {
            CvtPaletteToGray(decoder->palette, gray_palette, 1 << decoder->bpp);
        }
        bgr = realloc(bgr, (decoder->width * 3 + 32) * sizeof(char));
        assert(bgr != NULL);
    }

    fseek(decoder->file, decoder->offset, SEEK_SET);
    switch (decoder->bpp)
    {
        /************************* 1 BPP ************************/
        case 1 :
            for (y = 0; y < decoder->height; y++, data += step)
            {
                fread(src, sizeof(char), src_pitch, decoder->file);
                FillColorRow1(color ? data : bgr, src, decoder->width, decoder->palette);
                if (!color)
                    ihlCvt_BGR2Gray_8u_C3C1R(bgr, 0, data, 0, decoder->width, 1);
            }
            result = true;
            break;

        /************************* 4 BPP ************************/
        case 4 :
            if (decoder->rle_code == BMP_RGB)
            {
                for (y = 0; y < decoder->height; y++, data += step)
                {
                    fread(src, sizeof(char), src_pitch, decoder->file);
                    if (color)
                        FillColorRow4(data, src, decoder->width, decoder->palette);
                    else
                        FillGrayRow4(data, src, decoder->width, gray_palette);
                }
                result = true;
            }
            else if (decoder->rle_code == BMP_RLE4)    // rle4 compression
            {
                unsigned char* line_end = data + width3;
                y                       = 0;
                while (true)
                {
                    int code;
                    fread(&code, sizeof(short), 1, decoder->file);
                    const int len   = code & 255;
                    code          >>= 8;
                    if (len != 0)    // encoded mode
                    {
                        PaletteEntry  clr[2];
                        unsigned char gray_clr[2];
                        int           t    = 0;
                        clr[0]             = decoder->palette[code >> 4];
                        clr[1]             = decoder->palette[code & 15];
                        gray_clr[0]        = gray_palette[code >> 4];
                        gray_clr[1]        = gray_palette[code & 15];
                        unsigned char* end = data + len * nch;
                        if (end > line_end)
                            goto decode_rle4_bad;
                        do
                        {
                            if (color)
                                WRITE_PIX(data, clr[t]);
                            else
                                *data = gray_clr[t];
                            t ^= 1;
                        }
                        while ((data += nch) < end);
                    }
                    else if (code > 2)    // absolute mode
                    {
                        if (data + code * nch > line_end)
                            goto decode_rle4_bad;
                        int sz = (((code + 1) >> 1) + 1) & (~1);
                        //assert((size_t)sz < src.size());
                        fread(src, sizeof(char), sz, decoder->file);
                        if (color)
                            data = FillColorRow4(data, src, code, decoder->palette);
                        else
                            data = FillGrayRow4(data, src, code, gray_palette);
                    }
                    else
                    {
                        int x_shift3 = (int)(line_end - data);
                        if (code == 2)
                        {
                            fread(&x_shift3, sizeof(char), 1, decoder->file);
                            x_shift3 *= nch;
                            fseek(decoder->file, 1, SEEK_CUR);
                        }
                        if (color)
                            data = FillUniColor(data, &line_end, step, width3, &y, decoder->height, x_shift3, decoder->palette[0]);
                        else
                            data = FillUniGray(data, &line_end, step, width3, &y, decoder->height, x_shift3, gray_palette[0]);
                        if (y >= decoder->height)
                            break;
                    }
                }
                result = true;
            decode_rle4_bad:;
            }
            break;

        /************************* 8 BPP ************************/
        case 8 :
            if (decoder->rle_code == BMP_RGB)
            {
                for (y = 0; y < decoder->height; y++, data += step)
                {
                    fread(src, sizeof(char), src_pitch, decoder->file);
                    if (color)
                        FillColorRow8(data, src, decoder->width, decoder->palette);
                    else
                        FillGrayRow8(data, src, decoder->width, gray_palette);
                }
                result = true;
            }
            else if (decoder->rle_code == BMP_RLE8)    // rle8 compression
            {
                unsigned char* line_end      = data + width3;
                int            line_end_flag = 0;
                y                            = 0;
                while (true)
                {
                    int code;
                    fread(&code, sizeof(short), 1, decoder->file);
                    int len   = code & 255;
                    code    >>= 8;
                    if (len != 0)    // encoded mode
                    {
                        int prev_y  = y;
                        len        *= nch;
                        if (data + len > line_end)
                            goto decode_rle8_bad;
                        if (color)
                            data = FillUniColor(data, &line_end, step, width3, &y, decoder->height, len, decoder->palette[code]);
                        else
                            data = FillUniGray(data, &line_end, step, width3, &y, decoder->height, len, gray_palette[code]);
                        line_end_flag = y - prev_y;
                        if (y >= decoder->height)
                            break;
                    }
                    else if (code > 2)    // absolute mode
                    {
                        int prev_y = y;
                        int code3  = code * nch;
                        if (data + code3 > line_end) goto decode_rle8_bad;
                        int sz = (code + 1) & (~1);
                        // assert((size_t)sz < _src.size());
                        fread(src, sizeof(char), sz, decoder->file);
                        if (color)
                            data = FillColorRow8(data, src, code, decoder->palette);
                        else
                            data = FillGrayRow8(data, src, code, gray_palette);
                        line_end_flag = y - prev_y;
                    }
                    else
                    {
                        int x_shift3 = (int)(line_end - data);
                        int y_shift  = decoder->height - y;
                        if (code || !line_end_flag || x_shift3 < width3)
                        {
                            if (code == 2)
                            {
                                fread(&x_shift3, sizeof(char), 1, decoder->file);
                                x_shift3 *= nch;
                                fread(&y_shift, sizeof(char), 1, decoder->file);
                            }
                            x_shift3 += (y_shift * width3) & ((code == 0) - 1);
                            if (y >= decoder->height)
                                break;
                            if (color)
                                data = FillUniColor(data, &line_end, step, width3, &y, decoder->height, x_shift3, decoder->palette[0]);
                            else
                                data = FillUniGray(data, &line_end, step, width3, &y, decoder->height, x_shift3, gray_palette[0]);
                            if (y >= decoder->height)
                                break;
                        }
                        line_end_flag = 0;
                        if (y >= decoder->height)
                            break;
                    }
                }
                result = true;
            decode_rle8_bad:;
            }
            break;
        /************************* 15 BPP ************************/
        case 15 :
            for (y = 0; y < decoder->height; y++, data += step)
            {
                fread(src, sizeof(char), src_pitch, decoder->file);
                if (!color)
                    ihlCvt_BGR5552Gray_8u_C2C1R(src, 0, data, 0, decoder->width, 1);
                else
                    ihlCvt_BGR5552BGR_8u_C2C3R(src, 0, data, 0, decoder->width, 1);
            }
            result = true;
            break;
        /************************* 16 BPP ************************/
        case 16 :
            for (y = 0; y < decoder->height; y++, data += step)
            {
                fread(src, sizeof(char), src_pitch, decoder->file);
                if (!color)
                    ihlCvt_BGR5652Gray_8u_C2C1R(src, 0, data, 0, decoder->width, 1);
                else
                    ihlCvt_BGR5652BGR_8u_C2C3R(src, 0, data, 0, decoder->width, 1);
            }
            result = true;
            break;
        /************************* 24 BPP ************************/
        case 24 :
            for (y = 0; y < decoder->height; y++, data += step)
            {
                fread(src, sizeof(char), src_pitch, decoder->file);
                if (!color)
                    ihlCvt_BGR2Gray_8u_C3C1R(src, 0, data, 0, decoder->width, 1);
                else
                    memcpy(data, src, decoder->width * 3);
            }
            result = true;
            break;
        /************************* 32 BPP ************************/
        case 32 :
        {
            bool has_bit_mask = (decoder->rgba_bit_offset[0] >= 0) && (decoder->rgba_bit_offset[1] >= 0) && (decoder->rgba_bit_offset[2] >= 0);
            for (y = 0; y < decoder->height; y++, data += step)
            {
                fread(src, sizeof(char), src_pitch, decoder->file);
                if (!color)
                {
                    if (has_bit_mask)
                        maskBGRAtoGray(decoder, data, src, decoder->width);
                    else
                        ihlCvt_BGRA2Gray_8u_C4C1R(src, 0, data, 0, decoder->width, 1);
                }
                else if (channels(img) == 3)
                {
                    if (has_bit_mask)
                        maskBGRA(decoder, data, src, decoder->width, false);
                    else
                        ihlCvt_BGRA2BGR_8u_C4C3R(src, 0, data, 0, decoder->width, 1);
                }
                else if (channels(img) == 4)
                {
                    if (has_bit_mask)
                        maskBGRA(decoder, data, src, decoder->width, true);
                    else
                        memcpy(data, src, decoder->width * 4);
                }
            }
        }
            result = true;
            break;
        default : perror("Invalid/unsupported mode");
    }
    free(bgr);
    return result;
}

void close(BmpDecoder* decoder)
{
    fclose(decoder->file);
    decoder->file = NULL;
}

bool isFormatSupported(int depth)
{
    return depth == HL_8U;
}

void setDestination(BmpEncoder* encoder, const char* filename)
{
    encoder->filename = filename;
}

bool write(const BmpEncoder* encoder, const Mat* img)
{
    int           width = img->cols, height = img->rows, channel_s = channels(img);
    int           fileStep  = (width * channel_s + 3) & -4;
    unsigned char zeropad[] = "\0\0\0\0";
    // WLByteStream  strm;
    FILE* fp                = fopen(encoder->filename, "wb");

    if (!fp)
        return false;

    int          bitmapHeaderSize = 40;
    int          paletteSize      = channel_s > 1 ? 0 : 1024;
    int          headerSize       = 14 /* fileheader */ + bitmapHeaderSize + paletteSize;
    size_t       fileSize         = (size_t)fileStep * height + headerSize;
    PaletteEntry palette[256];

    // write signature 'BM'
    fwrite("BM", sizeof(short), 1, fp);    // 写入bfType

    // write file header
    int bfSize = validateToInt(fileSize);
    fwrite(&bfSize, sizeof(int), 1, fp);        // 写入bfSize
    fwrite(&zeropad, sizeof(short), 2, fp);     // 写入bfReserved1和bfReserved2
    fwrite(&headerSize, sizeof(int), 1, fp);    // 写入bfOffBits

    // write bitmap header
    fwrite(&bitmapHeaderSize, sizeof(int), 1, fp);    // 写入biSize
    fwrite(&width, sizeof(int), 1, fp);               // 写入biWidth
    fwrite(&height, sizeof(int), 1, fp);              // 写入biHeight
    short biPlanes = 1;
    fwrite(&biPlanes, sizeof(short), 1, fp);          // 写入biPlanes
    short biBitCount = channel_s << 3;
    fwrite(&biBitCount, sizeof(short), 1, fp);        // 写入biBitCount
    int biCompression = BMP_RGB;
    fwrite(&biCompression, sizeof(int), 1, fp);       // 写入biCompression
    fwrite(&zeropad, sizeof(int), 1, fp);             // 写入biSizeImage
    fwrite(&zeropad, sizeof(int), 1, fp);             // 写入biXPelsPerMeter
    fwrite(&zeropad, sizeof(int), 1, fp);             // 写入biYPelsPerMeter
    fwrite(&zeropad, sizeof(int), 1, fp);             // 写入biClrUsed
    fwrite(&zeropad, sizeof(int), 1, fp);             // 写入biClrImportant

    if (channel_s == 1)
    {
        FillGrayPalette(palette, 8, false);
        fwrite(palette, sizeof(char), sizeof(palette), fp);
    }

    width *= channel_s;
    for (int y = 0; y < height; ++y)
    {
        fwrite(PIXEL(*img, void, 0, y), sizeof(char), width, fp);
        if (fileStep > width)
            fwrite(zeropad, sizeof(char), fileStep - width, fp);
    }

    fclose(fp);
    return true;
}
