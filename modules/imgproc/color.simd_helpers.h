#pragma once

#define HL_DESCALE(x, n) (((x) + (1 << ((n) - 1))) >> (n))

//constants for conversion from/to RGB and Gray, YUV, YCrCb according to BT.601
const static float B2YF = 0.114f;
const static float G2YF = 0.587f;
const static float R2YF = 0.299f;

enum
{
    gray_shift = 15,
    yuv_shift  = 14,
    xyz_shift  = 12,
    R2Y        = 4899,     // == R2YF*16384
    G2Y        = 9617,     // == G2YF*16384
    B2Y        = 1868,     // == B2YF*16384
    RY15       = 9798,     // == R2YF*32768 + 0.5
    GY15       = 19235,    // == G2YF*32768 + 0.5
    BY15       = 3735,     // == B2YF*32768 + 0.5
    BLOCK_SIZE = 256
};

typedef enum SizePolicy
{
    TO_YUV,
    FROM_YUV,
    FROM_UYVY,
    TO_UYVY,
    NONE
} SizePolicy;

typedef struct CvtHelper
{
    Mat src;
    Mat dst;
    int depth;
    int scn;
} CvtHelper;

void initCvtHelper(CvtHelper* helper, const Mat* _src, Mat* _dst, int dcn, SizePolicy sizePolicy)
{
    assert(!empty(_src));

    int stype     = type(_src);
    helper->scn   = HL_MAT_CN(stype);
    helper->depth = HL_MAT_DEPTH(stype);

    if (_src == _dst)    // inplace processing (#6653)
        copyTo(_src, &helper->src);
    else
        helper->src = *_src;

    int swidth  = helper->src.cols;
    int sheight = helper->src.rows;
    int dwidth;
    int dheight;
    switch (sizePolicy)
    {
        case TO_YUV :
            assert(swidth % 2 == 0 && sheight % 2 == 0);
            dwidth  = swidth;
            dheight = sheight / 2 * 3;
            break;
        case FROM_YUV :
            assert(swidth % 2 == 0 && sheight % 3 == 0);
            dwidth  = swidth;
            dheight = sheight * 2 / 3;
            break;
        case FROM_UYVY :
        case TO_UYVY :
            assert(swidth % 2 == 0);
            dwidth  = swidth;
            dheight = sheight;
            break;
        case NONE :
        default :
            dwidth  = swidth;
            dheight = sheight;
            break;
    }
    create(_dst, dheight, dwidth, HL_MAKETYPE(helper->depth, dcn));
    helper->dst = *_dst;
}

typedef void (*CvtFunc)(const void* src, void* dst, int n);

void CvtColorLoop(const unsigned char* src_data, size_t src_step, unsigned char* dst_data, size_t dst_step, int width, int height, CvtFunc cvt)
{
    for (int y = 0; y < height; ++y, src_data += src_step, dst_data += dst_step)
    {
        cvt((const void*)src_data, (void*)dst_data, width);
    }
}