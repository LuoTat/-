#include "precomp.hxx"

namespace hl
{
namespace hal
{

extern const uchar popCountTable[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

const static uchar popCountTable2[]   = {0, 1, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 1, 2, 2, 2, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4, 2, 3, 3, 3, 3, 4, 4, 4, 3, 4, 4, 4, 3, 4, 4, 4};

const static uchar popCountTable4[]   = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

int normHamming(const uchar* a, int n, int cellSize)
{
    if (cellSize == 1) return normHamming(a, n);
    const uchar* tab = 0;
    if (cellSize == 2) tab = popCountTable2;
    else if (cellSize == 4) tab = popCountTable4;
    else return -1;
    int i      = 0;
    int result = 0;

    for (; i <= n - 4; i += 4) result += tab[a[i]] + tab[a[i + 1]] + tab[a[i + 2]] + tab[a[i + 3]];

    for (; i < n; i++) result += tab[a[i]];
    return result;
}

int normHamming(const uchar* a, const uchar* b, int n, int cellSize)
{
    if (cellSize == 1) return normHamming(a, b, n);
    const uchar* tab = 0;
    if (cellSize == 2) tab = popCountTable2;
    else if (cellSize == 4) tab = popCountTable4;
    else return -1;
    int i      = 0;
    int result = 0;

    for (; i <= n - 4; i += 4) result += tab[a[i] ^ b[i]] + tab[a[i + 1] ^ b[i + 1]] + tab[a[i + 2] ^ b[i + 2]] + tab[a[i + 3] ^ b[i + 3]];

    for (; i < n; i++) result += tab[a[i] ^ b[i]];
    return result;
}

float normL2Sqr_(const float* a, const float* b, int n)
{
    int   j = 0;
    float d = 0.f;

    for (; j < n; j++)
    {
        float t  = a[j] - b[j];
        d       += t * t;
    }
    return d;
}

float normL1_(const float* a, const float* b, int n)
{
    int   j = 0;
    float d = 0.f;

    for (; j < n; j++) d += std::abs(a[j] - b[j]);
    return d;
}

int normL1_(const uchar* a, const uchar* b, int n)
{
    int j = 0, d = 0;

    for (; j < n; j++) d += std::abs(a[j] - b[j]);
    return d;
}

}    // namespace hal

//==================================================================================================

template <typename T, typename ST>
int normInf_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result = std::max(result, normInf<T, ST>(src, len * cn)); }
    else
    {
        for (int i = 0; i < len; i++, src += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++) result = std::max(result, ST(hl_abs(src[k])));
            }
    }
    *_result = result;
    return 0;
}

template <typename T, typename ST>
int normL1_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result += normL1<T, ST>(src, len * cn); }
    else
    {
        for (int i = 0; i < len; i++, src += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++) result += hl_abs(src[k]);
            }
    }
    *_result = result;
    return 0;
}

template <typename T, typename ST>
int normL2_(const T* src, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result += normL2Sqr<T, ST>(src, len * cn); }
    else
    {
        for (int i = 0; i < len; i++, src += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++)
                {
                    T v     = src[k];
                    result += (ST)v * v;
                }
            }
    }
    *_result = result;
    return 0;
}

template <typename T, typename ST>
int normDiffInf_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result = std::max(result, normInf<T, ST>(src1, src2, len * cn)); }
    else
    {
        for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++) result = std::max(result, (ST)std::abs(src1[k] - src2[k]));
            }
    }
    *_result = result;
    return 0;
}

template <typename T, typename ST>
int normDiffL1_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result += normL1<T, ST>(src1, src2, len * cn); }
    else
    {
        for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++) result += std::abs(src1[k] - src2[k]);
            }
    }
    *_result = result;
    return 0;
}

template <typename T, typename ST>
int normDiffL2_(const T* src1, const T* src2, const uchar* mask, ST* _result, int len, int cn)
{
    ST result = *_result;
    if (!mask) { result += normL2Sqr<T, ST>(src1, src2, len * cn); }
    else
    {
        for (int i = 0; i < len; i++, src1 += cn, src2 += cn)
            if (mask[i])
            {
                for (int k = 0; k < cn; k++)
                {
                    ST v    = src1[k] - src2[k];
                    result += v * v;
                }
            }
    }
    *_result = result;
    return 0;
}

#define HL_DEF_NORM_FUNC(L, suffix, type, ntype)                                                                                               \
    static int norm##L##_##suffix(const type* src, const uchar* mask, ntype* r, int len, int cn) { return norm##L##_(src, mask, r, len, cn); } \
    static int normDiff##L##_##suffix(const type* src1, const type* src2, const uchar* mask, ntype* r, int len, int cn) { return normDiff##L##_(src1, src2, mask, r, (int)len, cn); }

#define HL_DEF_NORM_ALL(suffix, type, inftype, l1type, l2type) \
    HL_DEF_NORM_FUNC(Inf, suffix, type, inftype)               \
    HL_DEF_NORM_FUNC(L1, suffix, type, l1type)                 \
    HL_DEF_NORM_FUNC(L2, suffix, type, l2type)

HL_DEF_NORM_ALL(8u, uchar, int, int, int)
HL_DEF_NORM_ALL(8s, schar, int, int, int)
HL_DEF_NORM_ALL(16u, ushort, int, int, double)
HL_DEF_NORM_ALL(16s, short, int, int, double)

// TODO: 无法处理std::abs(uint)
// HL_DEF_NORM_ALL(32u, uint, uint, double, double)
HL_DEF_NORM_ALL(32s, int, int, double, double)
HL_DEF_NORM_ALL(32f, float, float, double, double)
HL_DEF_NORM_ALL(64f, double, double, double, double)

typedef int (*NormFunc)(const uchar*, const uchar*, uchar*, int, int);
typedef int (*NormDiffFunc)(const uchar*, const uchar*, const uchar*, uchar*, int, int);

static NormFunc getNormFunc(int normType, int depth)
{
    static NormFunc normTab[3][8] = {{(NormFunc)normInf_8u, (NormFunc)normInf_8s, (NormFunc)normInf_16u, (NormFunc)normInf_16s, (NormFunc)0, (NormFunc)normInf_32s, (NormFunc)normInf_32f, (NormFunc)normInf_64f},
                                     {(NormFunc)normL1_8u, (NormFunc)normL1_8s, (NormFunc)normL1_16u, (NormFunc)normL1_16s, (NormFunc)0, (NormFunc)normL1_32s, (NormFunc)normL1_32f, (NormFunc)normL1_64f},
                                     {(NormFunc)normL2_8u, (NormFunc)normL2_8s, (NormFunc)normL2_16u, (NormFunc)normL2_16s, (NormFunc)0, (NormFunc)normL2_32s, (NormFunc)normL2_32f, (NormFunc)normL2_64f}};

    return normTab[normType][depth];
}

static NormDiffFunc getNormDiffFunc(int normType, int depth)
{
    static NormDiffFunc normDiffTab[3][8] = {{(NormDiffFunc)normDiffInf_8u, (NormDiffFunc)normDiffInf_8s, (NormDiffFunc)normDiffInf_16u, (NormDiffFunc)normDiffInf_16s, (NormDiffFunc)0, (NormDiffFunc)normDiffInf_32s, (NormDiffFunc)normDiffInf_32f, (NormDiffFunc)normDiffInf_64f},
                                             {(NormDiffFunc)normDiffL1_8u, (NormDiffFunc)normDiffL1_8s, (NormDiffFunc)normDiffL1_16u, (NormDiffFunc)normDiffL1_16s, (NormDiffFunc)0, (NormDiffFunc)normDiffL1_32s, (NormDiffFunc)normDiffL1_32f, (NormDiffFunc)normDiffL1_64f},
                                             {(NormDiffFunc)normDiffL2_8u, (NormDiffFunc)normDiffL2_8s, (NormDiffFunc)normDiffL2_16u, (NormDiffFunc)normDiffL2_16s, (NormDiffFunc)0, (NormDiffFunc)normDiffL2_32s, (NormDiffFunc)normDiffL2_32f, (NormDiffFunc)normDiffL2_64f}};

    return normDiffTab[normType][depth];
}

double norm(const Mat& _src, int normType, const Mat& _mask)
{
    normType &= NORM_TYPE_MASK;
    HL_Assert(normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR || ((normType == NORM_HAMMING || normType == NORM_HAMMING2) && _src.type() == HL_8U));

    Mat src = _src, mask = _mask;

    int depth = src.depth(), cn = src.channels();
    if (src.isContinuous() && mask.empty())
    {
        size_t len = src.total() * cn;
        if (len == (size_t)(int)len)
        {
            if (depth == HL_32F)
            {
                const float* data = src.ptr<float>();

                if (normType == NORM_L2)
                {
                    double result = 0;
                    normL2_32f(data, 0, &result, (int)len, 1);
                    return std::sqrt(result);
                }
                if (normType == NORM_L2SQR)
                {
                    double result = 0;
                    normL2_32f(data, 0, &result, (int)len, 1);
                    return result;
                }
                if (normType == NORM_L1)
                {
                    double result = 0;
                    normL1_32f(data, 0, &result, (int)len, 1);
                    return result;
                }
                if (normType == NORM_INF)
                {
                    float result = 0;
                    normInf_32f(data, 0, &result, (int)len, 1);
                    return result;
                }
            }
            if (depth == HL_8U)
            {
                const uchar* data = src.ptr<uchar>();

                if (normType == NORM_HAMMING) { return hal::normHamming(data, (int)len); }

                if (normType == NORM_HAMMING2) { return hal::normHamming(data, (int)len, 2); }
            }
        }
    }

    HL_Assert(mask.empty() || mask.type() == HL_8U);

    if (normType == NORM_HAMMING || normType == NORM_HAMMING2)
    {
        if (!mask.empty())
        {
            Mat temp;
            bitwise_and(src, mask, temp);
            return norm(temp, normType);
        }
        int cellSize             = normType == NORM_HAMMING ? 1 : 2;

        const Mat*      arrays[] = {&src, 0};
        uchar*          ptrs[1]  = {};
        NAryMatIterator it(arrays, ptrs);
        int             total  = (int)it.size;
        int             result = 0;

        for (size_t i = 0; i < it.nplanes; i++, ++it) { result += hal::normHamming(ptrs[0], total, cellSize); }

        return result;
    }

    NormFunc func = getNormFunc(normType >> 1, depth);
    HL_Assert(func != 0);

    const Mat* arrays[] = {&src, &mask, 0};
    uchar*     ptrs[2]  = {};

    union
    {
        double d;
        int    i;
        float  f;
    } result;

    result.d = 0;
    NAryMatIterator it(arrays, ptrs);
    HL_CheckLT((size_t)it.size, (size_t)INT_MAX, "");

    if ((normType == NORM_L1 && depth <= HL_16S) || ((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= HL_8S))
    {
        // special case to handle "integer" overflow in accumulator
        const size_t esz             = src.elemSize();
        const int    total           = (int)it.size;
        const int    intSumBlockSize = (normType == NORM_L1 && depth <= HL_8S ? (1 << 23) : (1 << 15)) / cn;
        const int    blockSize       = std::min(total, intSumBlockSize);
        int          isum            = 0;
        int          count           = 0;

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (int j = 0; j < total; j += blockSize)
            {
                int bsz = std::min(total - j, blockSize);
                func(ptrs[0], ptrs[1], (uchar*)&isum, bsz, cn);
                count += bsz;
                if (count + blockSize >= intSumBlockSize || (i + 1 >= it.nplanes && j + bsz >= total))
                {
                    result.d += isum;
                    isum      = 0;
                    count     = 0;
                }
                ptrs[0] += bsz * esz;
                if (ptrs[1]) ptrs[1] += bsz;
            }
        }
    }
    else
    {
        // generic implementation
        for (size_t i = 0; i < it.nplanes; i++, ++it) { func(ptrs[0], ptrs[1], (uchar*)&result, (int)it.size, cn); }
    }

    if (normType == NORM_INF)
    {
        if (depth == HL_64F) return result.d;
        else if (depth == HL_32F) return result.f;
        else return result.i;
    }
    else if (normType == NORM_L2) return std::sqrt(result.d);

    return result.d;
}

double norm(const Mat& _src1, const Mat& _src2, int normType, const Mat& _mask)
{
    HL_CheckTypeEQ(_src1.type(), _src2.type(), "Input type mismatch");
    HL_Assert(_src1.sameSize(_src2));

    if (normType & HL_RELATIVE) { return norm(_src1, _src2, normType & ~HL_RELATIVE, _mask) / (norm(_src2, normType, _mask) + DBL_EPSILON); }

    Mat src1 = _src1, src2 = _src2, mask = _mask;
    int depth = src1.depth(), cn = src1.channels();

    normType &= 7;
    HL_Assert(normType == NORM_INF || normType == NORM_L1 || normType == NORM_L2 || normType == NORM_L2SQR || ((normType == NORM_HAMMING || normType == NORM_HAMMING2) && src1.type() == HL_8U));

    if (src1.isContinuous() && src2.isContinuous() && mask.empty())
    {
        size_t len = src1.total() * src1.channels();
        if (len == (size_t)(int)len)
        {
            if (src1.depth() == HL_32F)
            {
                const float* data1 = src1.ptr<float>();
                const float* data2 = src2.ptr<float>();

                if (normType == NORM_L2)
                {
                    double result = 0;
                    normDiffL2_32f(data1, data2, 0, &result, (int)len, 1);
                    return std::sqrt(result);
                }
                if (normType == NORM_L2SQR)
                {
                    double result = 0;
                    normDiffL2_32f(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
                if (normType == NORM_L1)
                {
                    double result = 0;
                    normDiffL1_32f(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
                if (normType == NORM_INF)
                {
                    float result = 0;
                    normDiffInf_32f(data1, data2, 0, &result, (int)len, 1);
                    return result;
                }
            }
        }
    }

    HL_Assert(mask.empty() || mask.type() == HL_8U);

    if (normType == NORM_HAMMING || normType == NORM_HAMMING2)
    {
        if (!mask.empty())
        {
            Mat temp;
            bitwise_xor(src1, src2, temp);
            bitwise_and(temp, mask, temp);
            return norm(temp, normType);
        }
        int cellSize             = normType == NORM_HAMMING ? 1 : 2;

        const Mat*      arrays[] = {&src1, &src2, 0};
        uchar*          ptrs[2]  = {};
        NAryMatIterator it(arrays, ptrs);
        int             total  = (int)it.size;
        int             result = 0;

        for (size_t i = 0; i < it.nplanes; i++, ++it) { result += hal::normHamming(ptrs[0], ptrs[1], total, cellSize); }

        return result;
    }

    NormDiffFunc func = getNormDiffFunc(normType >> 1, depth);
    HL_Assert(func != 0);

    const Mat* arrays[] = {&src1, &src2, &mask, 0};
    uchar*     ptrs[3]  = {};

    union
    {
        double   d;
        float    f;
        int      i;
        unsigned u;
    } result;

    result.d = 0;
    NAryMatIterator it(arrays, ptrs);
    HL_CheckLT((size_t)it.size, (size_t)INT_MAX, "");

    if ((normType == NORM_L1 && depth <= HL_16S) || ((normType == NORM_L2 || normType == NORM_L2SQR) && depth <= HL_8S))
    {
        // special case to handle "integer" overflow in accumulator
        const size_t esz             = src1.elemSize();
        const int    total           = (int)it.size;
        const int    intSumBlockSize = (normType == NORM_L1 && depth <= HL_8S ? (1 << 23) : (1 << 15)) / cn;
        const int    blockSize       = std::min(total, intSumBlockSize);
        int          isum            = 0;
        int          count           = 0;

        for (size_t i = 0; i < it.nplanes; i++, ++it)
        {
            for (int j = 0; j < total; j += blockSize)
            {
                int bsz = std::min(total - j, blockSize);
                func(ptrs[0], ptrs[1], ptrs[2], (uchar*)&isum, bsz, cn);
                count += bsz;
                if (count + blockSize >= intSumBlockSize || (i + 1 >= it.nplanes && j + bsz >= total))
                {
                    result.d += isum;
                    isum      = 0;
                    count     = 0;
                }
                ptrs[0] += bsz * esz;
                ptrs[1] += bsz * esz;
                if (ptrs[2]) ptrs[2] += bsz;
            }
        }
    }
    else
    {
        // generic implementation
        for (size_t i = 0; i < it.nplanes; i++, ++it) { func(ptrs[0], ptrs[1], ptrs[2], (uchar*)&result, (int)it.size, cn); }
    }

    if (normType == NORM_INF)
    {
        if (depth == HL_64F) return result.d;
        else if (depth == HL_32F) return result.f;
        else return result.u;
    }
    else if (normType == NORM_L2) return std::sqrt(result.d);

    return result.d;
}

void normalize(const Mat& _src, Mat& _dst, double a, double b, int norm_type, int rtype, const Mat& _mask)
{
    double scale = 1, shift = 0;

    if (rtype < 0) rtype = _dst.depth();

    if (norm_type == HL_MINMAX)
    {
        double smin = 0, smax = 0;
        double dmin = MIN(a, b), dmax = MAX(a, b);
        minMaxIdx(_src, &smin, &smax, 0, 0, _mask);
        scale = (dmax - dmin) * (smax - smin > DBL_EPSILON ? 1. / (smax - smin) : 0);
        if (rtype == HL_32F)
        {
            scale = (float)scale;
            shift = (float)dmin - (float)(smin * scale);
        }
        else shift = dmin - smin * scale;
    }
    else if (norm_type == HL_L2 || norm_type == HL_L1 || norm_type == HL_C)
    {
        scale = norm(_src, norm_type, _mask);
        scale = scale > DBL_EPSILON ? a / scale : 0.;
        shift = 0;
    }
    else HL_Error(HL_StsBadArg, "Unknown/unsupported norm type");

    Mat src = _src;
    if (_mask.empty()) src.convertTo(_dst, rtype, scale, shift);
    else
    {
        Mat temp;
        src.convertTo(temp, rtype, scale, shift);
        temp.copyTo(_dst, _mask);
    }
}

}    // namespace hl