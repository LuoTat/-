#include "precomp.hxx"
#include <iostream>

namespace hl
{

////////////////// Helper functions //////////////////////

#define HL_CLAMP_INT(v, vmin, vmax) (v < vmin ? vmin : (vmax < v ? vmax : v))

static const size_t OUT_OF_RANGE = (size_t)1 << (sizeof(size_t) * 8 - 2);

static void calcHistLookupTables_8u(const Mat& hist, int dims, const float** ranges, const double* uniranges, bool uniform, std::vector<size_t>& _tab)
{
    const int low = 0, high = 256;
    int       i, j;
    _tab.resize((high - low) * dims);
    size_t* tab = &_tab[0];

    if (uniform)
    {
        for (i = 0; i < dims; i++)
        {
            double a    = uniranges[i * 2];
            double b    = uniranges[i * 2 + 1];
            int    sz   = hist.size[i];
            size_t step = hist.step[i];

            double v_lo = ranges ? ranges[i][0] : 0;
            double v_hi = ranges ? ranges[i][1] : 256;

            for (j = low; j < high; j++)
            {
                int    idx         = hlFloor(j * a + b);
                size_t written_idx = OUT_OF_RANGE;
                if (j >= v_lo && j < v_hi)
                {
                    idx         = HL_CLAMP_INT(idx, 0, sz - 1);
                    written_idx = idx * step;
                }
                tab[i * (high - low) + j - low] = written_idx;
            }
        }
    }
    else if (ranges)
    {
        for (i = 0; i < dims; i++)
        {
            int    limit = std::min(hlCeil(ranges[i][0]), high);
            int    idx = -1, sz = hist.size[i];
            size_t written_idx = OUT_OF_RANGE;
            size_t step        = hist.step[i];

            for (j = low;;)
            {
                for (; j < limit; j++)
                    tab[i * (high - low) + j - low] = written_idx;

                if ((unsigned)(++idx) < (unsigned)sz)
                {
                    limit       = std::min(hlCeil(ranges[i][idx + 1]), high);
                    written_idx = idx * step;
                }
                else
                {
                    for (; j < high; j++)
                        tab[i * (high - low) + j - low] = OUT_OF_RANGE;
                    break;
                }
            }
        }
    }
    else
    {
        HL_Error(Error::StsBadArg, "Either ranges, either uniform ranges should be provided");
    }
}

static void histPrepareImages(const Mat* images, int nimages, const int* channels, const Mat& mask, int dims, const int* histSize, const float** ranges, bool uniform, std::vector<uchar*>& ptrs, std::vector<int>& deltas, Size& imsize, std::vector<double>& uniranges)
{
    int i, j, c;
    HL_Assert(channels != 0 || nimages == dims);

    imsize     = images[0].size();
    int  depth = images[0].depth(), esz1 = (int)images[0].elemSize1();
    bool isContinuous = true;

    ptrs.resize(dims + 1);
    deltas.resize((dims + 1) * 2);

    for (i = 0; i < dims; i++)
    {
        if (!channels)
        {
            j = i;
            c = 0;
            HL_Assert(images[j].channels() == 1);
        }
        else
        {
            c = channels[i];
            HL_Assert(c >= 0);
            for (j = 0; j < nimages; c -= images[j].channels(), j++)
                if (c < images[j].channels())
                    break;
            HL_Assert(j < nimages);
        }

        HL_Assert(images[j].size() == imsize && images[j].depth() == depth);
        if (!images[j].isContinuous())
            isContinuous = false;
        ptrs[i]           = images[j].data + c * esz1;
        deltas[i * 2]     = images[j].channels();
        deltas[i * 2 + 1] = (int)(images[j].step / esz1 - imsize.width * deltas[i * 2]);
    }

    if (!mask.empty())
    {
        HL_Assert(mask.size() == imsize && mask.channels() == 1);
        isContinuous         = isContinuous && mask.isContinuous();
        ptrs[dims]           = mask.data;
        deltas[dims * 2]     = 1;
        deltas[dims * 2 + 1] = (int)(mask.step / mask.elemSize1());
    }

    if (isContinuous)
    {
        imsize.width  *= imsize.height;
        imsize.height  = 1;
    }

    if (!ranges)    // implicit uniform ranges for 8U
    {
        HL_Assert(depth == HL_8U);

        uniranges.resize(dims * 2);
        for (i = 0; i < dims; i++)
        {
            uniranges[i * 2]     = histSize[i] / 256.;
            uniranges[i * 2 + 1] = 0;
        }
    }
    else if (uniform)
    {
        uniranges.resize(dims * 2);
        for (i = 0; i < dims; i++)
        {
            HL_Assert(ranges[i] && ranges[i][0] < ranges[i][1]);
            double low = ranges[i][0], high = ranges[i][1];
            double t             = histSize[i] / (high - low);
            uniranges[i * 2]     = t;
            uniranges[i * 2 + 1] = -t * low;
        }
    }
    else
    {
        for (i = 0; i < dims; i++)
        {
            size_t n = histSize[i];
            for (size_t k = 0; k < n; k++)
                HL_Assert(ranges[i][k] < ranges[i][k + 1]);
        }
    }
}

////////////////////////////////// C A L C U L A T E    H I S T O G R A M ////////////////////////////////////

template <typename T>
static void calcHist_(std::vector<uchar*>& _ptrs, const std::vector<int>& _deltas, Size imsize, Mat& hist, int dims, const float** _ranges, const double* _uniranges, bool uniform)
{
    T**          ptrs   = (T**)&_ptrs[0];
    const int*   deltas = &_deltas[0];
    uchar*       H      = hist.ptr();
    int          i, x;
    const uchar* mask  = _ptrs[dims];
    int          mstep = _deltas[dims * 2 + 1];
    int          size[HL_MAX_DIM];
    size_t       hstep[HL_MAX_DIM];

    for (i = 0; i < dims; i++)
    {
        size[i]  = hist.size[i];
        hstep[i] = hist.step[i];
    }

    if (uniform)
    {
        const double* uniranges = &_uniranges[0];

        if (dims == 1)
        {
            double   a = uniranges[0], b = uniranges[1];
            int      sz = size[0], d0 = deltas[0], step0 = deltas[1];
            const T* p0  = (const T*)ptrs[0];

            double v0_lo = _ranges[0][0];
            double v0_hi = _ranges[0][1];

            for (; imsize.height--; p0 += step0, mask += mstep)
            {
                if (!mask)
                    for (x = 0; x < imsize.width; x++, p0 += d0)
                    {
                        double v0  = (double)*p0;
                        int    idx = hlFloor(v0 * a + b);
                        if (v0 < v0_lo || v0 >= v0_hi)
                            continue;
                        idx = HL_CLAMP_INT(idx, 0, sz - 1);
                        HL_DbgAssert((unsigned)idx < (unsigned)sz);
                        ((int*)H)[idx]++;
                    }
                else
                    for (x = 0; x < imsize.width; x++, p0 += d0)
                        if (mask[x])
                        {
                            double v0  = (double)*p0;
                            int    idx = hlFloor(v0 * a + b);
                            if (v0 < v0_lo || v0 >= v0_hi)
                                continue;
                            idx = HL_CLAMP_INT(idx, 0, sz - 1);
                            HL_DbgAssert((unsigned)idx < (unsigned)sz);
                            ((int*)H)[idx]++;
                        }
            }
            return;
        }
        else if (dims == 2)
        {
            double a0 = uniranges[0], b0 = uniranges[1], a1 = uniranges[2], b1 = uniranges[3];
            int    sz0 = size[0], sz1 = size[1];
            int    d0 = deltas[0], step0 = deltas[1],
                d1 = deltas[2], step1 = deltas[3];
            size_t   hstep0 = hstep[0];
            const T* p0     = (const T*)ptrs[0];
            const T* p1     = (const T*)ptrs[1];

            double v0_lo    = _ranges[0][0];
            double v0_hi    = _ranges[0][1];
            double v1_lo    = _ranges[1][0];
            double v1_hi    = _ranges[1][1];

            for (; imsize.height--; p0 += step0, p1 += step1, mask += mstep)
            {
                if (!mask)
                    for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1)
                    {
                        double v0   = (double)*p0;
                        double v1   = (double)*p1;
                        int    idx0 = hlFloor(v0 * a0 + b0);
                        int    idx1 = hlFloor(v1 * a1 + b1);
                        if (v0 < v0_lo || v0 >= v0_hi)
                            continue;
                        if (v1 < v1_lo || v1 >= v1_hi)
                            continue;
                        idx0 = HL_CLAMP_INT(idx0, 0, sz0 - 1);
                        idx1 = HL_CLAMP_INT(idx1, 0, sz1 - 1);
                        HL_DbgAssert((unsigned)idx0 < (unsigned)sz0 && (unsigned)idx1 < (unsigned)sz1);
                        ((int*)(H + hstep0 * idx0))[idx1]++;
                    }
                else
                    for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1)
                        if (mask[x])
                        {
                            double v0   = (double)*p0;
                            double v1   = (double)*p1;
                            int    idx0 = hlFloor(v0 * a0 + b0);
                            int    idx1 = hlFloor(v1 * a1 + b1);
                            if (v0 < v0_lo || v0 >= v0_hi)
                                continue;
                            if (v1 < v1_lo || v1 >= v1_hi)
                                continue;
                            idx0 = HL_CLAMP_INT(idx0, 0, sz0 - 1);
                            idx1 = HL_CLAMP_INT(idx1, 0, sz1 - 1);
                            HL_DbgAssert((unsigned)idx0 < (unsigned)sz0 && (unsigned)idx1 < (unsigned)sz1);
                            ((int*)(H + hstep0 * idx0))[idx1]++;
                        }
            }
            return;
        }
        else if (dims == 3)
        {
            double a0 = uniranges[0], b0 = uniranges[1],
                   a1 = uniranges[2], b1 = uniranges[3],
                   a2 = uniranges[4], b2 = uniranges[5];
            int sz0 = size[0], sz1 = size[1], sz2 = size[2];
            int d0 = deltas[0], step0 = deltas[1],
                d1 = deltas[2], step1 = deltas[3],
                d2 = deltas[4], step2 = deltas[5];
            size_t   hstep0 = hstep[0], hstep1 = hstep[1];
            const T* p0  = (const T*)ptrs[0];
            const T* p1  = (const T*)ptrs[1];
            const T* p2  = (const T*)ptrs[2];

            double v0_lo = _ranges[0][0];
            double v0_hi = _ranges[0][1];
            double v1_lo = _ranges[1][0];
            double v1_hi = _ranges[1][1];
            double v2_lo = _ranges[2][0];
            double v2_hi = _ranges[2][1];

            for (; imsize.height--; p0 += step0, p1 += step1, p2 += step2, mask += mstep)
            {
                if (!mask)
                    for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1, p2 += d2)
                    {
                        double v0   = (double)*p0;
                        double v1   = (double)*p1;
                        double v2   = (double)*p2;
                        int    idx0 = hlFloor(v0 * a0 + b0);
                        int    idx1 = hlFloor(v1 * a1 + b1);
                        int    idx2 = hlFloor(v2 * a2 + b2);
                        if (v0 < v0_lo || v0 >= v0_hi)
                            continue;
                        if (v1 < v1_lo || v1 >= v1_hi)
                            continue;
                        if (v2 < v2_lo || v2 >= v2_hi)
                            continue;
                        idx0 = HL_CLAMP_INT(idx0, 0, sz0 - 1);
                        idx1 = HL_CLAMP_INT(idx1, 0, sz1 - 1);
                        idx2 = HL_CLAMP_INT(idx2, 0, sz2 - 1);
                        HL_DbgAssert(
                            (unsigned)idx0 < (unsigned)sz0 && (unsigned)idx1 < (unsigned)sz1 && (unsigned)idx2 < (unsigned)sz2);
                        ((int*)(H + hstep0 * idx0 + hstep1 * idx1))[idx2]++;
                    }
                else
                    for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1, p2 += d2)
                        if (mask[x])
                        {
                            double v0   = (double)*p0;
                            double v1   = (double)*p1;
                            double v2   = (double)*p2;
                            int    idx0 = hlFloor(v0 * a0 + b0);
                            int    idx1 = hlFloor(v1 * a1 + b1);
                            int    idx2 = hlFloor(v2 * a2 + b2);
                            if (v0 < v0_lo || v0 >= v0_hi)
                                continue;
                            if (v1 < v1_lo || v1 >= v1_hi)
                                continue;
                            if (v2 < v2_lo || v2 >= v2_hi)
                                continue;
                            idx0 = HL_CLAMP_INT(idx0, 0, sz0 - 1);
                            idx1 = HL_CLAMP_INT(idx1, 0, sz1 - 1);
                            idx2 = HL_CLAMP_INT(idx2, 0, sz2 - 1);
                            HL_DbgAssert(
                                (unsigned)idx0 < (unsigned)sz0 && (unsigned)idx1 < (unsigned)sz1 && (unsigned)idx2 < (unsigned)sz2);
                            ((int*)(H + hstep0 * idx0 + hstep1 * idx1))[idx2]++;
                        }
            }
        }
        else
        {
            for (; imsize.height--; mask += mstep)
            {
                if (!mask)
                    for (x = 0; x < imsize.width; x++)
                    {
                        uchar* Hptr = H;
                        for (i = 0; i < dims; i++)
                        {
                            double v_lo = _ranges[i][0];
                            double v_hi = _ranges[i][1];
                            double v    = *ptrs[i];
                            if (v < v_lo || v >= v_hi)
                                break;
                            int idx = hlFloor(v * uniranges[i * 2] + uniranges[i * 2 + 1]);
                            idx     = HL_CLAMP_INT(idx, 0, size[i] - 1);
                            HL_DbgAssert((unsigned)idx < (unsigned)size[i]);
                            ptrs[i] += deltas[i * 2];
                            Hptr    += idx * hstep[i];
                        }

                        if (i == dims)
                            ++*((int*)Hptr);
                        else
                            for (; i < dims; i++)
                                ptrs[i] += deltas[i * 2];
                    }
                else
                    for (x = 0; x < imsize.width; x++)
                    {
                        uchar* Hptr = H;
                        i           = 0;
                        if (mask[x])
                            for (; i < dims; i++)
                            {
                                double v_lo = _ranges[i][0];
                                double v_hi = _ranges[i][1];
                                double v    = *ptrs[i];
                                if (v < v_lo || v >= v_hi)
                                    break;
                                int idx = hlFloor(v * uniranges[i * 2] + uniranges[i * 2 + 1]);
                                idx     = HL_CLAMP_INT(idx, 0, size[i] - 1);
                                HL_DbgAssert((unsigned)idx < (unsigned)size[i]);
                                ptrs[i] += deltas[i * 2];
                                Hptr    += idx * hstep[i];
                            }

                        if (i == dims)
                            ++*((int*)Hptr);
                        else
                            for (; i < dims; i++)
                                ptrs[i] += deltas[i * 2];
                    }
                for (i = 0; i < dims; i++)
                    ptrs[i] += deltas[i * 2 + 1];
            }
        }
    }
    else if (_ranges)
    {
        // non-uniform histogram
        const float* ranges[HL_MAX_DIM];
        for (i = 0; i < dims; i++)
            ranges[i] = &_ranges[i][0];

        for (; imsize.height--; mask += mstep)
        {
            for (x = 0; x < imsize.width; x++)
            {
                uchar* Hptr = H;
                i           = 0;

                if (!mask || mask[x])
                    for (; i < dims; i++)
                    {
                        float        v   = (float)*ptrs[i];
                        const float* R   = ranges[i];
                        int          idx = -1, sz = size[i];

                        while (v >= R[idx + 1] && ++idx < sz);    // nop

                        if ((unsigned)idx >= (unsigned)sz)
                            break;

                        ptrs[i] += deltas[i * 2];
                        Hptr    += idx * hstep[i];
                    }

                if (i == dims)
                    ++*((int*)Hptr);
                else
                    for (; i < dims; i++)
                        ptrs[i] += deltas[i * 2];
            }

            for (i = 0; i < dims; i++)
                ptrs[i] += deltas[i * 2 + 1];
        }
    }
    else
    {
        HL_Error(Error::StsBadArg, "Either ranges, either uniform ranges should be provided");
    }
}

static void calcHist_8u(std::vector<uchar*>& _ptrs, const std::vector<int>& _deltas, Size imsize, Mat& hist, int dims, const float** _ranges, const double* _uniranges, bool uniform)
{
    uchar**             ptrs   = &_ptrs[0];
    const int*          deltas = &_deltas[0];
    uchar*              H      = hist.ptr();
    int                 x;
    const uchar*        mask  = _ptrs[dims];
    int                 mstep = _deltas[dims * 2 + 1];
    std::vector<size_t> _tab;

    calcHistLookupTables_8u(hist, dims, _ranges, _uniranges, uniform, _tab);
    const size_t* tab = &_tab[0];

    if (dims == 1)
    {
        int d0 = deltas[0], step0 = deltas[1];
        int matH[256] = {
            0,
        };
        const uchar* p0 = (const uchar*)ptrs[0];

        for (; imsize.height--; p0 += step0, mask += mstep)
        {
            if (!mask)
            {
                if (d0 == 1)
                {
                    for (x = 0; x <= imsize.width - 4; x += 4)
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
                    for (x = 0; x <= imsize.width - 4; x += 4)
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

                for (; x < imsize.width; x++, p0 += d0)
                    matH[*p0]++;
            }
            else
                for (x = 0; x < imsize.width; x++, p0 += d0)
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
    else if (dims == 2)
    {
        int d0 = deltas[0], step0 = deltas[1],
            d1 = deltas[2], step1 = deltas[3];
        const uchar* p0 = (const uchar*)ptrs[0];
        const uchar* p1 = (const uchar*)ptrs[1];

        for (; imsize.height--; p0 += step0, p1 += step1, mask += mstep)
        {
            if (!mask)
                for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1)
                {
                    size_t idx = tab[*p0] + tab[*p1 + 256];
                    if (idx < OUT_OF_RANGE)
                        ++*(int*)(H + idx);
                }
            else
                for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1)
                {
                    size_t idx;
                    if (mask[x] && (idx = tab[*p0] + tab[*p1 + 256]) < OUT_OF_RANGE)
                        ++*(int*)(H + idx);
                }
        }
    }
    else if (dims == 3)
    {
        int d0 = deltas[0], step0 = deltas[1],
            d1 = deltas[2], step1 = deltas[3],
            d2 = deltas[4], step2 = deltas[5];

        const uchar* p0 = (const uchar*)ptrs[0];
        const uchar* p1 = (const uchar*)ptrs[1];
        const uchar* p2 = (const uchar*)ptrs[2];

        for (; imsize.height--; p0 += step0, p1 += step1, p2 += step2, mask += mstep)
        {
            if (!mask)
                for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1, p2 += d2)
                {
                    size_t idx = tab[*p0] + tab[*p1 + 256] + tab[*p2 + 512];
                    if (idx < OUT_OF_RANGE)
                        ++*(int*)(H + idx);
                }
            else
                for (x = 0; x < imsize.width; x++, p0 += d0, p1 += d1, p2 += d2)
                {
                    size_t idx;
                    if (mask[x] && (idx = tab[*p0] + tab[*p1 + 256] + tab[*p2 + 512]) < OUT_OF_RANGE)
                        ++*(int*)(H + idx);
                }
        }
    }
    else
    {
        for (; imsize.height--; mask += mstep)
        {
            if (!mask)
                for (x = 0; x < imsize.width; x++)
                {
                    uchar* Hptr = H;
                    int    i    = 0;
                    for (; i < dims; i++)
                    {
                        size_t idx = tab[*ptrs[i] + i * 256];
                        if (idx >= OUT_OF_RANGE)
                            break;
                        Hptr    += idx;
                        ptrs[i] += deltas[i * 2];
                    }

                    if (i == dims)
                        ++*((int*)Hptr);
                    else
                        for (; i < dims; i++)
                            ptrs[i] += deltas[i * 2];
                }
            else
                for (x = 0; x < imsize.width; x++)
                {
                    uchar* Hptr = H;
                    int    i    = 0;
                    if (mask[x])
                        for (; i < dims; i++)
                        {
                            size_t idx = tab[*ptrs[i] + i * 256];
                            if (idx >= OUT_OF_RANGE)
                                break;
                            Hptr    += idx;
                            ptrs[i] += deltas[i * 2];
                        }

                    if (i == dims)
                        ++*((int*)Hptr);
                    else
                        for (; i < dims; i++)
                            ptrs[i] += deltas[i * 2];
                }
            for (int i = 0; i < dims; i++)
                ptrs[i] += deltas[i * 2 + 1];
        }
    }
}

void drawHist_T(const Mat& hist, Mat& histImage, uint width, uint height, uchar thresh)
{
    Mat hist_norm;
    normalize(hist, hist_norm, 0, height, NORM_MINMAX, HL_32U);
    histImage.create(height, width * hist_norm.rows, HL_8UC3);

    for (int y = histImage.rows - 1; y >= 0; --y)
    {
        for (int x = 0; x < histImage.cols; x += width)
        {
            uint& pixi = hist_norm.at<uint>(x / width);
            if (pixi == 0)
            {
                memset(histImage.ptr(y, x), 255, 3 * width);
            }
            else
            {
                memset(histImage.ptr(y, x), 0, 3 * width);
                --pixi;
            }
        }
    }
    for (int y = 0; y < histImage.rows; ++y)
    {
        for (uint x = thresh * width; x < thresh * width + width; ++x)
        {
            uchar* ptr = histImage.ptr(y, x);
            ptr[0]     = 0;
            ptr[1]     = 0;
            ptr[2]     = 255;
        }
    }
}

void drawHist(const Mat& hist, Mat& histImage, uint width, uint height)
{
    Mat hist_norm;
    normalize(hist, hist_norm, 0, height, NORM_MINMAX, HL_32U);
    histImage.create(height, width * hist_norm.rows, HL_8UC1);

    for (int y = histImage.rows - 1; y >= 0; --y)
    {
        for (int x = 0; x < histImage.cols; x += width)
        {
            uint& pixi = hist_norm.at<uint>(x / width);
            if (pixi == 0)
            {
                memset(histImage.ptr(y, x), 255, width);
            }
            else
            {
                memset(histImage.ptr(y, x), 0, width);
                --pixi;
            }
        }
    }
}

}    // namespace hl

void hl::calcHist(const Mat* images, int nimages, const int* channels, const Mat& _mask, Mat& _hist, int dims, const int* histSize, const float** ranges, bool uniform, bool accumulate)
{
    HL_Assert(images && nimages > 0);

    Mat mask = _mask;

    HL_Assert(dims > 0 && histSize);

    const uchar* const histdata = _hist.ptr();
    _hist.create(dims, histSize, HL_32F);
    Mat hist = _hist;

    if (histdata != hist.data)
        accumulate = false;

    Mat ihist   = hist;
    ihist.flags = (ihist.flags & ~HL_MAT_TYPE_MASK) | HL_32S;

    if (!accumulate)
        hist = Scalar(0.);
    else
        hist.convertTo(ihist, HL_32S);

    std::vector<uchar*> ptrs;
    std::vector<int>    deltas;
    std::vector<double> uniranges;
    Size                imsize;

    HL_Assert(mask.empty() || mask.type() == HL_8UC1);
    histPrepareImages(images, nimages, channels, mask, dims, hist.size, ranges, uniform, ptrs, deltas, imsize, uniranges);
    const double* _uniranges = uniform ? &uniranges[0] : 0;

    int depth                = images[0].depth();

    if (depth == HL_8U)
        calcHist_8u(ptrs, deltas, imsize, ihist, dims, ranges, _uniranges, uniform);
    else if (depth == HL_16U)
        calcHist_<ushort>(ptrs, deltas, imsize, ihist, dims, ranges, _uniranges, uniform);
    else if (depth == HL_32F)
        calcHist_<float>(ptrs, deltas, imsize, ihist, dims, ranges, _uniranges, uniform);
    else
        HL_Error(HL_StsUnsupportedFormat, "");

    ihist.convertTo(hist, HL_32F);
}

class EqualizeHistCalcHist_Invoker: public hl::ParallelLoopBody
{
public:
    enum
    {
        HIST_SZ = 256
    };

    EqualizeHistCalcHist_Invoker(hl::Mat& src, int* histogram, hl::Mutex* histogramLock):
        src_(src), globalHistogram_(histogram), histogramLock_(histogramLock)
    {}

    void operator()(const hl::Range& rowRange) const override
    {
        int localHistogram[HIST_SZ] = {
            0,
        };

        const size_t sstep = src_.step;

        int width          = src_.cols;
        int height         = rowRange.end - rowRange.start;

        if (src_.isContinuous())
        {
            width  *= height;
            height  = 1;
        }

        for (const uchar* ptr = src_.ptr<uchar>(rowRange.start); height--; ptr += sstep)
        {
            int x = 0;
            for (; x <= width - 4; x += 4)
            {
                int t0 = ptr[x], t1 = ptr[x + 1];
                localHistogram[t0]++;
                localHistogram[t1]++;
                t0 = ptr[x + 2];
                t1 = ptr[x + 3];
                localHistogram[t0]++;
                localHistogram[t1]++;
            }

            for (; x < width; ++x)
                localHistogram[ptr[x]]++;
        }

        hl::AutoLock lock(*histogramLock_);

        for (int i = 0; i < HIST_SZ; i++)
            globalHistogram_[i] += localHistogram[i];
    }

    static bool isWorthParallel(const hl::Mat& src)
    {
        return (src.total() >= 640 * 480);
    }

private:
    EqualizeHistCalcHist_Invoker& operator=(const EqualizeHistCalcHist_Invoker&);

    hl::Mat&   src_;
    int*       globalHistogram_;
    hl::Mutex* histogramLock_;
};

class EqualizeHistLut_Invoker: public hl::ParallelLoopBody
{
public:
    EqualizeHistLut_Invoker(hl::Mat& src, hl::Mat& dst, int* lut):
        src_(src),
        dst_(dst),
        lut_(lut)
    {}

    void operator()(const hl::Range& rowRange) const override
    {
        const size_t sstep = src_.step;
        const size_t dstep = dst_.step;

        int  width         = src_.cols;
        int  height        = rowRange.end - rowRange.start;
        int* lut           = lut_;

        if (src_.isContinuous() && dst_.isContinuous())
        {
            width  *= height;
            height  = 1;
        }

        const uchar* sptr = src_.ptr<uchar>(rowRange.start);
        uchar*       dptr = dst_.ptr<uchar>(rowRange.start);

        for (; height--; sptr += sstep, dptr += dstep)
        {
            int x = 0;
            for (; x <= width - 4; x += 4)
            {
                int v0      = sptr[x];
                int v1      = sptr[x + 1];
                int x0      = lut[v0];
                int x1      = lut[v1];
                dptr[x]     = (uchar)x0;
                dptr[x + 1] = (uchar)x1;

                v0          = sptr[x + 2];
                v1          = sptr[x + 3];
                x0          = lut[v0];
                x1          = lut[v1];
                dptr[x + 2] = (uchar)x0;
                dptr[x + 3] = (uchar)x1;
            }

            for (; x < width; ++x)
                dptr[x] = (uchar)lut[sptr[x]];
        }
    }

    static bool isWorthParallel(const hl::Mat& src)
    {
        return (src.total() >= 640 * 480);
    }

private:
    EqualizeHistLut_Invoker& operator=(const EqualizeHistLut_Invoker&);

    hl::Mat& src_;
    hl::Mat& dst_;
    int*     lut_;
};

void hl::equalizeHist(const Mat& _src, Mat& _dst)
{
    HL_Assert(_src.type() == HL_8UC1);

    if (_src.empty())
        return;

    Mat src = _src;
    _dst.create(src.size(), src.type());
    Mat dst = _dst;

    Mutex histogramLockInstance;

    const int hist_sz       = EqualizeHistCalcHist_Invoker::HIST_SZ;
    int       hist[hist_sz] = {
        0,
    };
    int lut[hist_sz];

    EqualizeHistCalcHist_Invoker calcBody(src, hist, &histogramLockInstance);
    EqualizeHistLut_Invoker      lutBody(src, dst, lut);
    hl::Range                    heightRange(0, src.rows);

    if (EqualizeHistCalcHist_Invoker::isWorthParallel(src))
        parallel_for_(heightRange, calcBody);
    else
        calcBody(heightRange);

    int i = 0;
    while (!hist[i]) ++i;

    int total = (int)src.total();
    if (hist[i] == total)
    {
        dst.setTo(i);
        return;
    }

    float scale = (hist_sz - 1.f) / (total - hist[i]);
    int   sum   = 0;

    for (lut[i++] = 0; i < hist_sz; ++i)
    {
        sum    += hist[i];
        lut[i]  = saturate_cast<uchar>(sum * scale);
    }

    if (EqualizeHistLut_Invoker::isWorthParallel(src))
        parallel_for_(heightRange, lutBody);
    else
        lutBody(heightRange);
}