#include "precomp.h"

#define HL_CLAMP_INT(v, vmin, vmax) (v < vmin ? vmin : (vmax < v ? vmax : v))

const static size_t OUT_OF_RANGE = (size_t)1 << (sizeof(size_t) * 8 - 2);

static void calcHistLookupTables_8u(const Mat* hist, const float** ranges, const double* uniranges, bool uniform, size_t _tab[256])
{
    const int low = 0, high = 256;
    int       j;
    size_t*   tab = _tab;

    if (uniform)
    {
        double a    = uniranges[0];
        double b    = uniranges[1];
        int    sz   = hist->rows;
        size_t step = hist->step[0];

        double v_lo = ranges ? ranges[0][0] : 0;
        double v_hi = ranges ? ranges[0][1] : 256;

        for (j = low; j < high; j++)
        {
            int    idx         = hlFloor64f32s(j * a + b);
            size_t written_idx = OUT_OF_RANGE;
            if (j >= v_lo && j < v_hi)
            {
                idx         = HL_CLAMP_INT(idx, 0, sz - 1);
                written_idx = idx * step;
            }
            tab[j - low] = written_idx;
        }
    }
    else if (ranges)
    {
        int    limit = min(hlCeil32f32s(ranges[0][0]), high);
        int    idx = -1, sz = hist->rows;
        size_t written_idx = OUT_OF_RANGE;
        size_t step        = hist->step[0];

        for (j = low;;)
        {
            for (; j < limit; j++)
                tab[j - low] = written_idx;

            if ((unsigned int)(++idx) < (unsigned int)sz)
            {
                limit       = min(hlCeil32f32s(ranges[0][idx + 1]), high);
                written_idx = idx * step;
            }
            else
            {
                for (; j < high; j++)
                    tab[j - low] = OUT_OF_RANGE;
                break;
            }
        }
    }
    else
    {
        perror("Either ranges, either uniform ranges should be provided");
    }
}

static void histPrepareImages(const Mat* images, const int histSize, const float** ranges, bool uniform, unsigned char* ptrs[2], int deltas[4], int* width, int* height, double uniranges[2])
{
    *width     = images->cols;
    *height    = images->rows;
    int idepth = depth(images), esz1 = (int)elemSize1(images);

    ptrs[0]    = images->data;
    deltas[0]  = channels(images);
    deltas[1]  = (int)(images->step[0] / esz1 - *width * deltas[0]);

    *width    *= *height;
    *height    = 1;

    if (!ranges)    // implicit uniform ranges for 8U
    {
        assert(idepth == HL_8U);

        uniranges[0] = histSize / 256.0;
        uniranges[1] = 0;
    }
    else if (uniform)
    {
        assert(ranges[0] && ranges[0][0] < ranges[0][1]);
        double low = ranges[0][0], high = ranges[0][1];
        double t     = histSize / (high - low);
        uniranges[0] = t;
        uniranges[1] = -t * low;
#if 0    // This should be true by math, but it is not accurate numerically
            assert(hlFloor(low * uniranges[0] + uniranges[1]) == 0);
            assert((high * uniranges[0] + uniranges[1]) < histSize);
#endif
    }
    else
    {
        size_t n = histSize;
        for (size_t k = 0; k < n; ++k)
            assert(ranges[0][k] < ranges[0][k + 1]);
    }
}

#define DEF_CALCHIST_FUNC(suffix, T)                                                                                                                                          \
    static void calcHist_##suffix(unsigned char** _ptrs, const int* _deltas, int width, int height, Mat* hist, const float** _ranges, const double* _uniranges, bool uniform) \
    {                                                                                                                                                                         \
        T**                  ptrs   = (T**)_ptrs;                                                                                                                             \
        const int*           deltas = _deltas;                                                                                                                                \
        unsigned char*       H      = hist->data;                                                                                                                             \
        int                  x;                                                                                                                                               \
        const unsigned char* mask  = _ptrs[1];                                                                                                                                \
        int                  mstep = _deltas[3];                                                                                                                              \
        int                  size[2];                                                                                                                                         \
        size_t               hstep[2];                                                                                                                                        \
        size[0]  = hist->rows;                                                                                                                                                \
        hstep[0] = hist->step[0];                                                                                                                                             \
        if (uniform)                                                                                                                                                          \
        {                                                                                                                                                                     \
            const double* uniranges = _uniranges;                                                                                                                             \
            double        a = uniranges[0], b = uniranges[1];                                                                                                                 \
            int           sz = size[0], d0 = deltas[0], step0 = deltas[1];                                                                                                    \
            const T*      p0    = (const T*)ptrs[0];                                                                                                                          \
            double        v0_lo = _ranges[0][0];                                                                                                                              \
            double        v0_hi = _ranges[0][1];                                                                                                                              \
            for (; height--; p0 += step0, mask += mstep)                                                                                                                      \
            {                                                                                                                                                                 \
                if (!mask)                                                                                                                                                    \
                    for (x = 0; x < width; ++x, p0 += d0)                                                                                                                     \
                    {                                                                                                                                                         \
                        double v0  = (double)*p0;                                                                                                                             \
                        int    idx = hlFloor64f32s(v0 * a + b);                                                                                                               \
                        if (v0 < v0_lo || v0 >= v0_hi)                                                                                                                        \
                            continue;                                                                                                                                         \
                        idx = HL_CLAMP_INT(idx, 0, sz - 1);                                                                                                                   \
                        assert((unsigned)idx < (unsigned)sz);                                                                                                                 \
                        ((int*)H)[idx]++;                                                                                                                                     \
                    }                                                                                                                                                         \
                else                                                                                                                                                          \
                    for (x = 0; x < width; ++x, p0 += d0)                                                                                                                     \
                        if (mask[x])                                                                                                                                          \
                        {                                                                                                                                                     \
                            double v0  = (double)*p0;                                                                                                                         \
                            int    idx = hlFloor64f32s(v0 * a + b);                                                                                                           \
                            if (v0 < v0_lo || v0 >= v0_hi)                                                                                                                    \
                                continue;                                                                                                                                     \
                            idx = HL_CLAMP_INT(idx, 0, sz - 1);                                                                                                               \
                            assert((unsigned)idx < (unsigned)sz);                                                                                                             \
                            ((int*)H)[idx]++;                                                                                                                                 \
                        }                                                                                                                                                     \
            }                                                                                                                                                                 \
            return;                                                                                                                                                           \
        }                                                                                                                                                                     \
        else if (_ranges)                                                                                                                                                     \
        {                                                                                                                                                                     \
            /* non-uniform histogram */                                                                                                                                       \
            const float* ranges[2];                                                                                                                                           \
            ranges[0] = _ranges[0];                                                                                                                                           \
            for (; height--; mask += mstep)                                                                                                                                   \
            {                                                                                                                                                                 \
                for (x = 0; x < width; ++x)                                                                                                                                   \
                {                                                                                                                                                             \
                    unsigned char* Hptr = H;                                                                                                                                  \
                    if (!mask || mask[x])                                                                                                                                     \
                    {                                                                                                                                                         \
                        float        v   = (float)*ptrs[0];                                                                                                                   \
                        const float* R   = ranges[0];                                                                                                                         \
                        int          idx = -1, sz = size[0];                                                                                                                  \
                        while (v >= R[idx + 1] && ++idx < sz); /* nop */                                                                                                      \
                        if ((unsigned int)idx >= (unsigned int)sz)                                                                                                            \
                            break;                                                                                                                                            \
                        ptrs[0] += deltas[0];                                                                                                                                 \
                        Hptr    += idx * hstep[0];                                                                                                                            \
                    }                                                                                                                                                         \
                    ++*((int*)Hptr);                                                                                                                                          \
                }                                                                                                                                                             \
                ptrs[0] += deltas[1];                                                                                                                                         \
            }                                                                                                                                                                 \
        }                                                                                                                                                                     \
        else                                                                                                                                                                  \
        {                                                                                                                                                                     \
            perror("Either ranges, either uniform ranges should be provided");                                                                                                \
        }                                                                                                                                                                     \
    }

DEF_CALCHIST_FUNC(16u, unsigned short)
DEF_CALCHIST_FUNC(32f, float)

#define calcHist_(suffix, ptrs, deltas, width, height, ihist, ranges, _uniranges, uniform) \
    calcHist_##suffix(ptrs, deltas, width, height, ihist, ranges, _uniranges, uniform)

static void calcHist_8u(unsigned char** _ptrs, const int* _deltas, int width, int height, Mat* hist, const float** _ranges, const double* _uniranges, bool uniform)
{
    unsigned char**      ptrs   = _ptrs;
    const int*           deltas = _deltas;
    unsigned char*       H      = hist->data;
    int                  x;
    const unsigned char* mask      = _ptrs[1];
    int                  mstep     = _deltas[3];
    size_t               _tab[256] = {0};

    calcHistLookupTables_8u(hist, _ranges, _uniranges, uniform, _tab);
    const size_t*        tab = &_tab[0];
    int                  d0 = deltas[0], step0 = deltas[1];
    int                  matH[256] = {0};
    const unsigned char* p0        = (const unsigned char*)ptrs[0];

    for (; height--; p0 += step0, mask += mstep)
    {
        if (!mask)
        {
            if (d0 == 1)
            {
                for (x = 0; x <= width - 4; x += 4)
                {
                    int t0 = p0[x], t1 = p0[x + 1];
                    matH[t0]++;
                    matH[t1]++;
                    t0 = p0[x + 2];
                    t1 = p0[x + 3];
                    matH[t0]++;
                    matH[t1]++;
                }
                p0 += x;
            }
            else
                for (x = 0; x <= width - 4; x += 4)
                {
                    int t0 = p0[0], t1 = p0[d0];
                    matH[t0]++;
                    matH[t1]++;
                    p0 += d0 * 2;
                    t0  = p0[0];
                    t1  = p0[d0];
                    matH[t0]++;
                    matH[t1]++;
                    p0 += d0 * 2;
                }

            for (; x < width; x++, p0 += d0)
                matH[*p0]++;
        }
        else
            for (x = 0; x < width; x++, p0 += d0)
                if (mask[x])
                    matH[*p0]++;
    }

    for (int i = 0; i < 256; i++)
    {
        size_t hidx = tab[i];
        if (hidx < OUT_OF_RANGE)
            *(int*)(H + hidx) += matH[i];
    }
}

void calcHist(const Mat* images, Mat* _hist, const int histSize, const float** ranges, bool uniform, bool accumulate)
{
    assert(images);

    assert(histSize);

    const unsigned char* const histdata = _hist->data;
    create(_hist, 1, histSize, HL_32F);

    if (histdata != _hist->data)
        accumulate = false;    //TODO:Mat allocation is not supported

    Mat ihist   = *_hist;
    ihist.flags = (ihist.flags & ~HL_MAT_TYPE_MASK) | HL_32S;

    if (!accumulate)
        memset(_hist->data, 0, _hist->rows * _hist->step[0]);
    else
        convertTo(_hist, &ihist, HL_32S, 1, 0);

    unsigned char* ptrs[2]      = {NULL, NULL};
    int            deltas[4]    = {0, 0, 0, 0};
    double         uniranges[2] = {0, 0};
    int            width, height;

    histPrepareImages(images, histSize, ranges, uniform, ptrs, deltas, &width, &height, uniranges);
    const double* _uniranges = uniform ? uniranges : 0;

    int idepth               = depth(images);
    if (idepth == HL_8U)
        calcHist_8u(ptrs, deltas, width, height, &ihist, ranges, _uniranges, uniform);
    else if (idepth == HL_16U)
        calcHist_(16u, ptrs, deltas, width, height, &ihist, ranges, _uniranges, uniform);
    else if (idepth == HL_32F)
        calcHist_(32f, ptrs, deltas, width, height, &ihist, ranges, _uniranges, uniform);
    else
        perror("HL_StsUnsupportedFormat");

    convertTo(&ihist, _hist, HL_32F, 1, 0);
}

// void calcHist(const Mat* images, Mat* hist)
// {
//     if (images->channels != 1)
//     {
//         printf("Error: calcHist() only works with single channel images\n");
//         return;
//     }

//     // 创建一个256*1的灰度矩阵，元素类型为size_t
//     *hist = createMat(1, 256, 1, sizeof(size_t));

//     // 遍历所有的像素值
//     for (int y = 0; y < images->rows; ++y)
//     {
//         for (int x = 0; x < images->cols; ++x)
//         {
//             ++*(size_t*)PIXEL(*hist, *PIXEL(*images, x, y), 0);
//         }
//     }
// }

// void calcCDF(Mat* hist, double* cdf, int totalPixels)
// {
//     for (int x = 0; x < hist->cols; ++x)
//     {
//         size_t* hist_pixi = (size_t*)PIXEL(*hist, x, 0);
//         cdf[x]            = *hist_pixi / (double)totalPixels;
//     }

//     for (int i = 1; i < hist->cols; ++i)
//     {
//         cdf[i] = cdf[i - 1] + cdf[i];
//     }
// }

// void equalizeImage(Mat* image)
// {
//     Mat hist;

//     calcHist(image, &hist);

//     double cdf[256];    // 累积分布函数


//     // 2. 计算累积分布函数（CDF）
//     int totalPixels = image->rows * image->cols;
//     calcCDF(&hist, cdf, totalPixels);

//     // 3. 应用均衡化，更新像素值
//     for (int y = 0; y < image->rows; ++y)
//     {
//         for (int x = 0; x < image->cols; ++x)
//         {
//             unsigned char* pixel = PIXEL(*image, x, y);
//             *pixel               = (unsigned char)round((256 - 1) * cdf[*pixel]);
//         }
//     }
// }

// void drawHist(const Mat* hist, Mat* histImage, int width, int height)
// {
//     *histImage = createMat(height, 256 * width, 1, sizeof(unsigned char));

//     for (int y = 0; y < histImage->rows; ++y)
//     {
//         for (int x = 0; x < histImage->cols; x += width)
//         {
//             size_t* hist_pixi = (size_t*)PIXEL(*hist, x / width, 0);
//             if (*hist_pixi == 0)
//             {
//                 memset(PIXEL(*histImage, x, y), 255, width);
//             }
//             else
//             {
//                 memset(PIXEL(*histImage, x, y), 0, width);
//                 --*hist_pixi;
//             }
//         }
//     }
// }