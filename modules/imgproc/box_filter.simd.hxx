#include "precomp.hxx"

namespace hl
{
namespace cpu_baseline
{

// forward declarations
Ptr<BaseRowFilter>    getRowSumFilter(int srcType, int sumType, int ksize, int anchor);
Ptr<BaseColumnFilter> getColumnSumFilter(int sumType, int dstType, int ksize, int anchor, double scale);
Ptr<FilterEngine>     createBoxFilter(int srcType, int dstType, Size ksize, Point anchor, bool normalize, int borderType);

/****************************************************************************************\
                                         Box Filter
\****************************************************************************************/

namespace
{
template <typename T, typename ST>
struct RowSum: public BaseRowFilter
{
    RowSum(int _ksize, int _anchor):
        BaseRowFilter()
    {
        ksize  = _ksize;
        anchor = _anchor;
    }

    virtual void operator()(const uchar* src, uchar* dst, int width, int cn) override
    {
        const T* S = (const T*)src;
        ST*      D = (ST*)dst;
        int      i = 0, k, ksz_cn = ksize * cn;

        width = (width - 1) * cn;
        if (ksize == 3)
        {
            for (i = 0; i < width + cn; i++)
            {
                D[i] = (ST)S[i] + (ST)S[i + cn] + (ST)S[i + cn * 2];
            }
        }
        else if (ksize == 5)
        {
            for (i = 0; i < width + cn; i++)
            {
                D[i] = (ST)S[i] + (ST)S[i + cn] + (ST)S[i + cn * 2] + (ST)S[i + cn * 3] + (ST)S[i + cn * 4];
            }
        }
        else if (cn == 1)
        {
            ST s = 0;
            for (i = 0; i < ksz_cn; i++)
                s += (ST)S[i];
            D[0] = s;
            for (i = 0; i < width; i++)
            {
                s        += (ST)S[i + ksz_cn] - (ST)S[i];
                D[i + 1]  = s;
            }
        }
        else if (cn == 3)
        {
            ST s0 = 0, s1 = 0, s2 = 0;
            for (i = 0; i < ksz_cn; i += 3)
            {
                s0 += (ST)S[i];
                s1 += (ST)S[i + 1];
                s2 += (ST)S[i + 2];
            }
            D[0] = s0;
            D[1] = s1;
            D[2] = s2;
            for (i = 0; i < width; i += 3)
            {
                s0       += (ST)S[i + ksz_cn] - (ST)S[i];
                s1       += (ST)S[i + ksz_cn + 1] - (ST)S[i + 1];
                s2       += (ST)S[i + ksz_cn + 2] - (ST)S[i + 2];
                D[i + 3]  = s0;
                D[i + 4]  = s1;
                D[i + 5]  = s2;
            }
        }
        else if (cn == 4)
        {
            ST s0 = 0, s1 = 0, s2 = 0, s3 = 0;
            for (i = 0; i < ksz_cn; i += 4)
            {
                s0 += (ST)S[i];
                s1 += (ST)S[i + 1];
                s2 += (ST)S[i + 2];
                s3 += (ST)S[i + 3];
            }
            D[0] = s0;
            D[1] = s1;
            D[2] = s2;
            D[3] = s3;
            for (i = 0; i < width; i += 4)
            {
                s0       += (ST)S[i + ksz_cn] - (ST)S[i];
                s1       += (ST)S[i + ksz_cn + 1] - (ST)S[i + 1];
                s2       += (ST)S[i + ksz_cn + 2] - (ST)S[i + 2];
                s3       += (ST)S[i + ksz_cn + 3] - (ST)S[i + 3];
                D[i + 4]  = s0;
                D[i + 5]  = s1;
                D[i + 6]  = s2;
                D[i + 7]  = s3;
            }
        }
        else
            for (k = 0; k < cn; k++, S++, D++)
            {
                ST s = 0;
                for (i = 0; i < ksz_cn; i += cn)
                    s += (ST)S[i];
                D[0] = s;
                for (i = 0; i < width; i += cn)
                {
                    s         += (ST)S[i + ksz_cn] - (ST)S[i];
                    D[i + cn]  = s;
                }
            }
    }
};

template <typename ST, typename T>
struct ColumnSum: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int    i;
        ST*    SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(ST));

            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const ST* Sp = (const ST*)src[0];

                for (i = 0; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const ST* Sp = (const ST*)src[0];
            const ST* Sm = (const ST*)src[1 - ksize];
            T*        D  = (T*)dst;
            if (haveScale)
            {
                for (i = 0; i <= width - 2; i += 2)
                {
                    ST s0 = SUM[i] + Sp[i], s1 = SUM[i + 1] + Sp[i + 1];
                    D[i]        = saturate_cast<T>(s0 * _scale);
                    D[i + 1]    = saturate_cast<T>(s1 * _scale);
                    s0         -= Sm[i];
                    s1         -= Sm[i + 1];
                    SUM[i]      = s0;
                    SUM[i + 1]  = s1;
                }

                for (; i < width; i++)
                {
                    ST s0  = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<T>(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                for (i = 0; i <= width - 2; i += 2)
                {
                    ST s0 = SUM[i] + Sp[i], s1 = SUM[i + 1] + Sp[i + 1];
                    D[i]        = saturate_cast<T>(s0);
                    D[i + 1]    = saturate_cast<T>(s1);
                    s0         -= Sm[i];
                    s1         -= Sm[i + 1];
                    SUM[i]      = s0;
                    SUM[i + 1]  = s1;
                }

                for (; i < width; i++)
                {
                    ST s0  = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<T>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double          scale;
    int             sumCount;
    std::vector<ST> sum;
};

template <>
struct ColumnSum<int, uchar>: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int*   SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(int));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const int* Sp = (const int*)src[0];
                int        i  = 0;
                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1 - ksize];
            uchar*     D  = (uchar*)dst;
            if (haveScale)
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<uchar>(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<uchar>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double           scale;
    int              sumCount;
    std::vector<int> sum;
};

template <>
struct ColumnSum<ushort, uchar>: public BaseColumnFilter
{
    enum
    {
        SHIFT = 23
    };

    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
        divDelta = 0;
        divScale = 1;
        if (scale != 1)
        {
            int    d       = hlRound(1. / scale);
            double scalef  = ((double)(1 << SHIFT)) / d;
            divScale       = hlFloor(scalef);
            scalef        -= divScale;
            divDelta       = d / 2;
            if (scalef < 0.5)
                divDelta++;
            else
                divScale++;
        }
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        const int  ds = divScale;
        const int  dd = divDelta;
        ushort*    SUM;
        const bool haveScale = scale != 1;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(SUM[0]));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const ushort* Sp = (const ushort*)src[0];
                int           i  = 0;
                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const ushort* Sp = (const ushort*)src[0];
            const ushort* Sm = (const ushort*)src[1 - ksize];
            uchar*        D  = (uchar*)dst;
            if (haveScale)
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = (uchar)((s0 + dd) * ds >> SHIFT);
                    SUM[i] = (ushort)(s0 - Sm[i]);
                }
            }
            else
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<uchar>(s0);
                    SUM[i] = (ushort)(s0 - Sm[i]);
                }
            }
            dst += dststep;
        }
    }

    double              scale;
    int                 sumCount;
    int                 divDelta;
    int                 divScale;
    std::vector<ushort> sum;
};

template <>
struct ColumnSum<int, short>: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int    i;
        int*   SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(int));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const int* Sp = (const int*)src[0];
                i             = 0;
                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1 - ksize];
            short*     D  = (short*)dst;
            if (haveScale)
            {
                i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<short>(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<short>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double           scale;
    int              sumCount;
    std::vector<int> sum;
};

template <>
struct ColumnSum<int, ushort>: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int*   SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(int));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const int* Sp = (const int*)src[0];
                int        i  = 0;
                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1 - ksize];
            ushort*    D  = (ushort*)dst;
            if (haveScale)
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<ushort>(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<ushort>(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double           scale;
    int              sumCount;
    std::vector<int> sum;
};

template <>
struct ColumnSum<int, int>: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int*   SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(int));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const int* Sp = (const int*)src[0];
                int        i  = 0;
                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1 - ksize];
            int*       D  = (int*)dst;
            if (haveScale)
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = saturate_cast<int>(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                int i = 0;
                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = s0;
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double           scale;
    int              sumCount;
    std::vector<int> sum;
};

template <>
struct ColumnSum<int, float>: public BaseColumnFilter
{
    ColumnSum(int _ksize, int _anchor, double _scale):
        BaseColumnFilter()
    {
        ksize    = _ksize;
        anchor   = _anchor;
        scale    = _scale;
        sumCount = 0;
    }

    virtual void reset() override { sumCount = 0; }

    virtual void operator()(const uchar** src, uchar* dst, int dststep, int count, int width) override
    {
        int*   SUM;
        bool   haveScale = scale != 1;
        double _scale    = scale;

        if (width != (int)sum.size())
        {
            sum.resize(width);
            sumCount = 0;
        }

        SUM = &sum[0];
        if (sumCount == 0)
        {
            memset((void*)SUM, 0, width * sizeof(int));
            for (; sumCount < ksize - 1; sumCount++, src++)
            {
                const int* Sp = (const int*)src[0];
                int        i  = 0;

                for (; i < width; i++)
                    SUM[i] += Sp[i];
            }
        }
        else
        {
            HL_Assert(sumCount == ksize - 1);
            src += ksize - 1;
        }

        for (; count--; src++)
        {
            const int* Sp = (const int*)src[0];
            const int* Sm = (const int*)src[1 - ksize];
            float*     D  = (float*)dst;
            if (haveScale)
            {
                int i = 0;

                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = (float)(s0 * _scale);
                    SUM[i] = s0 - Sm[i];
                }
            }
            else
            {
                int i = 0;

                for (; i < width; i++)
                {
                    int s0 = SUM[i] + Sp[i];
                    D[i]   = (float)(s0);
                    SUM[i] = s0 - Sm[i];
                }
            }
            dst += dststep;
        }
    }

    double           scale;
    int              sumCount;
    std::vector<int> sum;
};

}    // namespace

Ptr<BaseRowFilter> getRowSumFilter(int srcType, int sumType, int ksize, int anchor)
{
    int sdepth = HL_MAT_DEPTH(srcType), ddepth = HL_MAT_DEPTH(sumType);
    HL_Assert(HL_MAT_CN(sumType) == HL_MAT_CN(srcType));

    if (anchor < 0)
        anchor = ksize / 2;

    if (sdepth == HL_8U && ddepth == HL_32S)
        return makePtr<RowSum<uchar, int>>(ksize, anchor);
    if (sdepth == HL_8U && ddepth == HL_16U)
        return makePtr<RowSum<uchar, ushort>>(ksize, anchor);
    if (sdepth == HL_8U && ddepth == HL_64F)
        return makePtr<RowSum<uchar, double>>(ksize, anchor);
    if (sdepth == HL_16U && ddepth == HL_32S)
        return makePtr<RowSum<ushort, int>>(ksize, anchor);
    if (sdepth == HL_16U && ddepth == HL_64F)
        return makePtr<RowSum<ushort, double>>(ksize, anchor);
    if (sdepth == HL_16S && ddepth == HL_32S)
        return makePtr<RowSum<short, int>>(ksize, anchor);
    if (sdepth == HL_32S && ddepth == HL_32S)
        return makePtr<RowSum<int, int>>(ksize, anchor);
    if (sdepth == HL_16S && ddepth == HL_64F)
        return makePtr<RowSum<short, double>>(ksize, anchor);
    if (sdepth == HL_32F && ddepth == HL_64F)
        return makePtr<RowSum<float, double>>(ksize, anchor);
    if (sdepth == HL_64F && ddepth == HL_64F)
        return makePtr<RowSum<double, double>>(ksize, anchor);

    HL_Error_(HL_StsNotImplemented, ("Unsupported combination of source format (={}), and buffer format (={})", srcType, sumType));
}

Ptr<BaseColumnFilter> getColumnSumFilter(int sumType, int dstType, int ksize, int anchor, double scale)
{
    int sdepth = HL_MAT_DEPTH(sumType), ddepth = HL_MAT_DEPTH(dstType);
    HL_Assert(HL_MAT_CN(sumType) == HL_MAT_CN(dstType));

    if (anchor < 0)
        anchor = ksize / 2;

    if (ddepth == HL_8U && sdepth == HL_32S)
        return makePtr<ColumnSum<int, uchar>>(ksize, anchor, scale);
    if (ddepth == HL_8U && sdepth == HL_16U)
        return makePtr<ColumnSum<ushort, uchar>>(ksize, anchor, scale);
    if (ddepth == HL_8U && sdepth == HL_64F)
        return makePtr<ColumnSum<double, uchar>>(ksize, anchor, scale);
    if (ddepth == HL_16U && sdepth == HL_32S)
        return makePtr<ColumnSum<int, ushort>>(ksize, anchor, scale);
    if (ddepth == HL_16U && sdepth == HL_64F)
        return makePtr<ColumnSum<double, ushort>>(ksize, anchor, scale);
    if (ddepth == HL_16S && sdepth == HL_32S)
        return makePtr<ColumnSum<int, short>>(ksize, anchor, scale);
    if (ddepth == HL_16S && sdepth == HL_64F)
        return makePtr<ColumnSum<double, short>>(ksize, anchor, scale);
    if (ddepth == HL_32S && sdepth == HL_32S)
        return makePtr<ColumnSum<int, int>>(ksize, anchor, scale);
    if (ddepth == HL_32F && sdepth == HL_32S)
        return makePtr<ColumnSum<int, float>>(ksize, anchor, scale);
    if (ddepth == HL_32F && sdepth == HL_64F)
        return makePtr<ColumnSum<double, float>>(ksize, anchor, scale);
    if (ddepth == HL_64F && sdepth == HL_32S)
        return makePtr<ColumnSum<int, double>>(ksize, anchor, scale);
    if (ddepth == HL_64F && sdepth == HL_64F)
        return makePtr<ColumnSum<double, double>>(ksize, anchor, scale);

    HL_Error_(HL_StsNotImplemented, ("Unsupported combination of sum format (={}), and destination format (={})", sumType, dstType));
}

Ptr<FilterEngine> createBoxFilter(int srcType, int dstType, Size ksize, Point anchor, bool normalize, int borderType)
{
    int sdepth = HL_MAT_DEPTH(srcType);
    int cn = HL_MAT_CN(srcType), sumType = HL_64F;
    if (sdepth == HL_8U && HL_MAT_DEPTH(dstType) == HL_8U && ksize.width * ksize.height <= 256)
        sumType = HL_16U;
    else if (sdepth <= HL_32S && (!normalize || ksize.width * ksize.height <= (sdepth == HL_8U ? (1 << 23) : sdepth == HL_16U ? (1 << 15)
                                                                                                                              : (1 << 16))))
        sumType = HL_32S;
    sumType                            = HL_MAKETYPE(sumType, cn);

    Ptr<BaseRowFilter>    rowFilter    = getRowSumFilter(srcType, sumType, ksize.width, anchor.x);
    Ptr<BaseColumnFilter> columnFilter = getColumnSumFilter(sumType,
                                                            dstType,
                                                            ksize.height,
                                                            anchor.y,
                                                            normalize ? 1. / (ksize.width * ksize.height) : 1);

    return makePtr<FilterEngine>(Ptr<BaseFilter>(), rowFilter, columnFilter, srcType, dstType, sumType, borderType);
}

}    // namespace cpu_baseline
}    // namespace hl