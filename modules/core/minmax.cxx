#include "precomp.hxx"

/****************************************************************************************\
*                                       minMaxLoc                                        *
\****************************************************************************************/

namespace hl
{

template <typename T, typename WT>
static void minMaxIdx_(const T* src, const uchar* mask, WT* _minVal, WT* _maxVal, size_t* _minIdx, size_t* _maxIdx, int len, size_t startIdx)
{
    WT     minVal = *_minVal, maxVal = *_maxVal;
    size_t minIdx = *_minIdx, maxIdx = *_maxIdx;

    if (!mask)
    {
        for (int i = 0; i < len; i++)
        {
            T val = src[i];
            if (val < minVal)
            {
                minVal = val;
                minIdx = startIdx + i;
            }
            if (val > maxVal)
            {
                maxVal = val;
                maxIdx = startIdx + i;
            }
        }
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            T val = src[i];
            if (mask[i] && val < minVal)
            {
                minVal = val;
                minIdx = startIdx + i;
            }
            if (mask[i] && val > maxVal)
            {
                maxVal = val;
                maxIdx = startIdx + i;
            }
        }
    }

    *_minIdx = minIdx;
    *_maxIdx = maxIdx;
    *_minVal = minVal;
    *_maxVal = maxVal;
}

static void minMaxIdx_8u(const uchar* src, const uchar* mask, int* minval, int* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_8s(const schar* src, const uchar* mask, int* minval, int* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_16u(const ushort* src, const uchar* mask, int* minval, int* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_16s(const short* src, const uchar* mask, int* minval, int* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_32u(const uint* src, const uchar* mask, uint* minval, uint* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_32s(const int* src, const uchar* mask, int* minval, int* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_32f(const float* src, const uchar* mask, float* minval, float* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

static void minMaxIdx_64f(const double* src, const uchar* mask, double* minval, double* maxval, size_t* minidx, size_t* maxidx, int len, size_t startidx)
{
    minMaxIdx_(src, mask, minval, maxval, minidx, maxidx, len, startidx);
}

typedef void (*MinMaxIdxFunc)(const uchar*, const uchar*, int*, int*, size_t*, size_t*, int, size_t);

static MinMaxIdxFunc getMinmaxTab(int depth)
{
    static MinMaxIdxFunc minmaxTab[] = {
        (MinMaxIdxFunc)(minMaxIdx_8u), (MinMaxIdxFunc)(minMaxIdx_8s), (MinMaxIdxFunc)(minMaxIdx_16u), (MinMaxIdxFunc)(minMaxIdx_16s), (MinMaxIdxFunc)(minMaxIdx_32u), (MinMaxIdxFunc)(minMaxIdx_32s), (MinMaxIdxFunc)(minMaxIdx_32f), (MinMaxIdxFunc)(minMaxIdx_64f)};

    return minmaxTab[depth];
}

static void ofs2idx(const Mat& a, size_t ofs, int* idx)
{
    int i, d = a.dims;
    if (ofs > 0)
    {
        ofs--;
        for (i = d - 1; i >= 0; i--)
        {
            int sz  = a.size[i];
            idx[i]  = (int)(ofs % sz);
            ofs    /= sz;
        }
    }
    else
    {
        for (i = d - 1; i >= 0; i--)
            idx[i] = -1;
    }
}
}    // namespace hl

void hl::minMaxIdx(const Mat& _src, double* minVal, double* maxVal, int* minIdx, int* maxIdx, const Mat& _mask)
{
    int type = _src.type(), depth = HL_MAT_DEPTH(type), cn = HL_MAT_CN(type);
    HL_Assert((cn == 1 && (_mask.empty() || _mask.type() == HL_8U)) || (cn > 1 && _mask.empty() && !minIdx && !maxIdx));

    Mat src = _src, mask = _mask;

    MinMaxIdxFunc func = getMinmaxTab(depth);
    HL_Assert(func != 0);

    const Mat*      arrays[] = {&src, &mask, 0};
    uchar*          ptrs[2]  = {};
    NAryMatIterator it(arrays, ptrs);

    size_t minidx = 0, maxidx = 0;
    int    iminval = INT_MAX, imaxval = INT_MIN;
    uint   uiminval = UINT_MAX, uimaxval = 0;
    float  fminval = std::numeric_limits<float>::infinity(), fmaxval = -fminval;
    double dminval = std::numeric_limits<double>::infinity(), dmaxval = -dminval;
    size_t startidx = 1;
    int *  minval = &iminval, *maxval = &imaxval;
    int    planeSize = (int)it.size * cn;

    if (depth == HL_32U)
        minval = (int*)&uiminval, maxval = (int*)&uimaxval;
    else if (depth == HL_32F)
        minval = (int*)&fminval, maxval = (int*)&fmaxval;
    else if (depth == HL_64F)
        minval = (int*)&dminval, maxval = (int*)&dmaxval;

    for (size_t i = 0; i < it.nplanes; i++, ++it, startidx += planeSize)
        func(ptrs[0], ptrs[1], minval, maxval, &minidx, &maxidx, planeSize, startidx);

    if (!src.empty() && mask.empty())
    {
        if (minidx == 0)
            minidx = 1;
        if (maxidx == 0)
            maxidx = 1;
    }

    if (minidx == 0)
        dminval = dmaxval = 0;
    else if (depth == HL_32F)
        dminval = fminval, dmaxval = fmaxval;
    else if (depth == HL_32U)
        dminval = uiminval, dmaxval = uimaxval;
    else if (depth <= HL_32S)
        dminval = iminval, dmaxval = imaxval;

    if (minVal)
        *minVal = dminval;
    if (maxVal)
        *maxVal = dmaxval;

    if (minIdx)
        ofs2idx(src, minidx, minIdx);
    if (maxIdx)
        ofs2idx(src, maxidx, maxIdx);
}