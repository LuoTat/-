#include "precomp.h"
#include "grfmt_bmp.h"
#include "utils.h"
#include <errno.h>
#include "openHL/imgcodecs.h"

#define HL_IO_MAX_IMAGE_WIDTH  1 << 20
#define HL_IO_MAX_IMAGE_HEIGHT 1 << 20
#define HL_IO_MAX_IMAGE_PIXELS 1 << 30

static void validateInputImageSize(const int width, const int height)
{
    assert(width > 0);
    assert((size_t)(width) <= HL_IO_MAX_IMAGE_WIDTH);
    assert(height > 0);
    assert((size_t)(height) <= HL_IO_MAX_IMAGE_HEIGHT);
    unsigned long pixels = (unsigned long)width * (unsigned long)height;
    assert(pixels <= HL_IO_MAX_IMAGE_PIXELS);
}

static bool imread_(const char* filename, int flags, Mat* img)
{
    BmpDecoder decoder = initBmpDecoder();

    // established the required input image size
    setSource(&decoder, filename);

    if (!readHeader(&decoder))
        return false;

    // established the required input image size
    validateInputImageSize(decoder.width, decoder.height);

    int type = decoder.type;
    if (flags != IMREAD_UNCHANGED)
    {
        if ((flags & IMREAD_ANYDEPTH) == 0)
            type = HL_MAKETYPE(HL_8U, HL_MAT_CN(type));

        if ((flags & IMREAD_COLOR) != 0 || ((flags & IMREAD_ANYCOLOR) != 0 && HL_MAT_CN(type) > 1))
            type = HL_MAKETYPE(HL_MAT_DEPTH(type), 3);
        else
            type = HL_MAKETYPE(HL_MAT_DEPTH(type), 1);
    }

    create(img, decoder.height, decoder.width, type);

    // read the image data
    bool success = false;

    if (readData(&decoder, img))
        success = true;

    if (!success)
    {
        release(img);
        return false;
    }

    close(&decoder);
    return true;
}

Mat imread(const char* filename, int flags)
{
    Mat img;
    imread_(filename, flags, &img);
    return img;
}

static bool imwrite_(const char* filename, Mat img)
{
    BmpEncoder encoder;

    // 确保输入图像不为空
    assert(!empty(&img));
    // 确保输入图像通道数为1、3或4
    assert(channels(&img) == 1 || channels(&img) == 3 || channels(&img) == 4);

    // 如果编码器不支持图像的深度，则将图像转换为 CV_8U 类型
    Mat temp;
    if (!isFormatSupported(depth(&img)))
    {
        assert(isFormatSupported(HL_8U));
        convertTo(&img, &temp, HL_8U, 1.0, 0.0);
        img = temp;
    }

    setDestination(&encoder, filename);

    bool code = false;

    code      = write(&encoder, &img);

    if (!code)
    {
        FILE* fp = fopen(filename, "wb");
        if (!fp)
        {
            if (errno == EACCES)
            {
                perror("imwrite_ can't open file for writing: permission denied");
            }
        }
        else
        {
            fclose(fp);
            remove(filename);
        }
    }
    return code;
}

bool imwrite(const char* filename, const Mat* img)
{
    assert(!empty(img));

    return imwrite_(filename, *img);
}