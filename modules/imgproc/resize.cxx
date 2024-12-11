#include "precomp.hxx"

#include "openHL/core/hal/intrin.hxx"
#include "openHL/core/utils/buffer_area.private.hxx"
#include "openHL/core/softfloat.hxx"
#include "fixedpoint.inl.hxx"

using namespace hl;

namespace
{

template <typename ET, bool needsign>
struct fixedtype
{
    typedef fixedpoint64 type;
};

template <>
struct fixedtype<uint32_t, false>
{
    typedef ufixedpoint64 type;
};

template <bool needsign>
struct fixedtype<int16_t, needsign>
{
    typedef fixedpoint32 type;
};

template <>
struct fixedtype<uint16_t, false>
{
    typedef ufixedpoint32 type;
};

template <bool needsign>
struct fixedtype<int8_t, needsign>
{
    typedef fixedpoint32 type;
};

template <>
struct fixedtype<uint8_t, false>
{
    typedef ufixedpoint16 type;
};

//FT is fixedtype<ET, needsign>::type
template <typename ET, typename FT, int n, bool mulall>
static void hlineResize(ET* src, int cn, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
{
    int i = 0;
    for (; i < dst_min; i++, m += n)    // Points that fall left from src image so became equal to leftmost src point
    {
        for (int j = 0; j < cn; j++, dst++)
        {
            *dst = src[j];
        }
    }
    for (; i < dst_max; i++, m += n)
    {
        ET* src_ofst = src + cn * ofst[i];
        for (int j = 0; j < cn; j++, dst++)
        {
            *dst = (mulall || !m[0].isZero()) ? m[0] * src_ofst[j] : FT::zero();
            for (int k = 1; k < n; k++)
            {
                *dst = *dst + ((mulall || !m[k].isZero()) ? m[k] * src_ofst[j + k * cn] : FT::zero());
            }
        }
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    ET* src_last = src + cn * ofst[dst_width - 1];
    for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
    {
        for (int j = 0; j < cn; j++, dst++)
        {
            *dst = src_last[j];
        }
    }
}

template <typename ET, typename FT, int n, bool mulall, int cncnt>
struct hline
{
    static void ResizeCn(ET* src, int cn, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        hlineResize<ET, FT, n, mulall>(src, cn, ofst, m, dst, dst_min, dst_max, dst_width);
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 2, true, 1>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]);
        for (; i < dst_min; i++, m += 2)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
        }
        for (; i < dst_max; i++, m += 2)
        {
            ET* px   = src + ofst[i];
            *(dst++) = m[0] * px[0] + m[1] * px[1];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + ofst[dst_width - 1])[0];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 2, true, 2>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]);
        for (; i < dst_min; i++, m += 2)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
        }
        for (; i < dst_max; i++, m += 2)
        {
            ET* px   = src + 2 * ofst[i];
            *(dst++) = m[0] * px[0] + m[1] * px[2];
            *(dst++) = m[0] * px[1] + m[1] * px[3];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 2 * ofst[dst_width - 1])[0];
        src1 = (src + 2 * ofst[dst_width - 1])[1];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 2, true, 3>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]), src2(src[2]);
        for (; i < dst_min; i++, m += 2)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
        }
        for (; i < dst_max; i++, m += 2)
        {
            ET* px   = src + 3 * ofst[i];
            *(dst++) = m[0] * px[0] + m[1] * px[3];
            *(dst++) = m[0] * px[1] + m[1] * px[4];
            *(dst++) = m[0] * px[2] + m[1] * px[5];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 3 * ofst[dst_width - 1])[0];
        src1 = (src + 3 * ofst[dst_width - 1])[1];
        src2 = (src + 3 * ofst[dst_width - 1])[2];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 2, true, 4>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]), src2(src[2]), src3(src[3]);
        for (; i < dst_min; i++, m += 2)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
            *(dst++) = src3;
        }
        for (; i < dst_max; i++, m += 2)
        {
            ET* px   = src + 4 * ofst[i];
            *(dst++) = m[0] * px[0] + m[1] * px[4];
            *(dst++) = m[0] * px[1] + m[1] * px[5];
            *(dst++) = m[0] * px[2] + m[1] * px[6];
            *(dst++) = m[0] * px[3] + m[1] * px[7];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 4 * ofst[dst_width - 1])[0];
        src1 = (src + 4 * ofst[dst_width - 1])[1];
        src2 = (src + 4 * ofst[dst_width - 1])[2];
        src3 = (src + 4 * ofst[dst_width - 1])[3];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
            *(dst++) = src3;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 4, true, 1>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]);
        for (; i < dst_min; i++, m += 4)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
        }
        for (; i < dst_max; i++, m += 4)
        {
            ET* px   = src + ofst[i];
            *(dst++) = m[0] * src[0] + m[1] * src[1] + m[2] * src[2] + m[3] * src[3];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + ofst[dst_width - 1])[0];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 4, true, 2>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]);
        for (; i < dst_min; i++, m += 4)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
        }
        for (; i < dst_max; i++, m += 4)
        {
            ET* px   = src + 2 * ofst[i];
            *(dst++) = m[0] * src[0] + m[1] * src[2] + m[2] * src[4] + m[3] * src[6];
            *(dst++) = m[0] * src[1] + m[1] * src[3] + m[2] * src[5] + m[3] * src[7];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 2 * ofst[dst_width - 1])[0];
        src1 = (src + 2 * ofst[dst_width - 1])[1];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 4, true, 3>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]), src2(src[2]);
        for (; i < dst_min; i++, m += 4)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
        }
        for (; i < dst_max; i++, m += 4)
        {
            ET* px   = src + 3 * ofst[i];
            *(dst++) = m[0] * src[0] + m[1] * src[3] + m[2] * src[6] + m[3] * src[9];
            *(dst++) = m[0] * src[1] + m[1] * src[4] + m[2] * src[7] + m[3] * src[10];
            *(dst++) = m[0] * src[2] + m[1] * src[5] + m[2] * src[8] + m[3] * src[11];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 3 * ofst[dst_width - 1])[0];
        src1 = (src + 3 * ofst[dst_width - 1])[1];
        src2 = (src + 3 * ofst[dst_width - 1])[2];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
        }
    }
};

template <typename ET, typename FT>
struct hline<ET, FT, 4, true, 4>
{
    static void ResizeCn(ET* src, int, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
    {
        int i = 0;
        FT  src0(src[0]), src1(src[1]), src2(src[2]), src3(src[3]);
        for (; i < dst_min; i++, m += 4)    // Points that fall left from src image so became equal to leftmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
            *(dst++) = src3;
        }
        for (; i < dst_max; i++, m += 4)
        {
            ET* px   = src + 4 * ofst[i];
            *(dst++) = m[0] * src[0] + m[1] * src[4] + m[2] * src[8] + m[3] * src[12];
            *(dst++) = m[0] * src[1] + m[1] * src[5] + m[2] * src[9] + m[3] * src[13];
            *(dst++) = m[0] * src[2] + m[1] * src[6] + m[2] * src[10] + m[3] * src[14];
            *(dst++) = m[0] * src[3] + m[1] * src[7] + m[2] * src[11] + m[3] * src[15];
        }
        // Avoid reading a potentially unset ofst, leading to a random memory read.
        if (i >= dst_width)
        {
            return;
        }
        src0 = (src + 4 * ofst[dst_width - 1])[0];
        src1 = (src + 4 * ofst[dst_width - 1])[1];
        src2 = (src + 4 * ofst[dst_width - 1])[2];
        src3 = (src + 4 * ofst[dst_width - 1])[3];
        for (; i < dst_width; i++)    // Points that fall right from src image so became equal to rightmost src point
        {
            *(dst++) = src0;
            *(dst++) = src1;
            *(dst++) = src2;
            *(dst++) = src3;
        }
    }
};

template <typename ET, typename FT, int n, bool mulall, int cncnt>
static void hlineResizeCn(ET* src, int cn, int* ofst, FT* m, FT* dst, int dst_min, int dst_max, int dst_width)
{
    hline<ET, FT, n, mulall, cncnt>::ResizeCn(src, cn, ofst, m, dst, dst_min, dst_max, dst_width);
}

template <>
void hlineResizeCn<uint8_t, ufixedpoint16, 2, true, 1>(uint8_t* src, int, int* ofst, ufixedpoint16* m, ufixedpoint16* dst, int dst_min, int dst_max, int dst_width)
{
    int           i = 0;
    ufixedpoint16 src_0(src[0]);
    for (; i < dst_min; i++, m += 2)
    {
        *(dst++) = src_0;
    }
    for (; i < dst_max; i += 1, m += 2)
    {
        uint8_t* px = src + ofst[i];
        *(dst++)    = m[0] * px[0] + m[1] * px[1];
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    src_0 = (src + ofst[dst_width - 1])[0];
    for (; i < dst_width; i++)
    {
        *(dst++) = src_0;
    }
}

template <>
void hlineResizeCn<uint8_t, ufixedpoint16, 2, true, 2>(uint8_t* src, int, int* ofst, ufixedpoint16* m, ufixedpoint16* dst, int dst_min, int dst_max, int dst_width)
{
    int i = 0;

    union
    {
        uint32_t d;
        uint16_t w[2];
    } srccn;

    ((ufixedpoint16*)(srccn.w))[0] = src[0];
    ((ufixedpoint16*)(srccn.w))[1] = src[1];
    for (; i < dst_min; i++, m += 2)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
    }
    for (; i < dst_max; i += 1, m += 2)
    {
        uint8_t* px = src + 2 * ofst[i];
        *(dst++)    = m[0] * px[0] + m[1] * px[2];
        *(dst++)    = m[0] * px[1] + m[1] * px[3];
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    ((ufixedpoint16*)(srccn.w))[0] = (src + 2 * ofst[dst_width - 1])[0];
    ((ufixedpoint16*)(srccn.w))[1] = (src + 2 * ofst[dst_width - 1])[1];
    for (; i < dst_width; i++)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
    }
}

template <>
void hlineResizeCn<uint8_t, ufixedpoint16, 2, true, 3>(uint8_t* src, int, int* ofst, ufixedpoint16* m, ufixedpoint16* dst, int dst_min, int dst_max, int dst_width)
{
    int i = 0;

    union
    {
        uint64_t q;
        uint16_t w[4];
    } srccn;

    ((ufixedpoint16*)(srccn.w))[0] = src[0];
    ((ufixedpoint16*)(srccn.w))[1] = src[1];
    ((ufixedpoint16*)(srccn.w))[2] = src[2];
    ((ufixedpoint16*)(srccn.w))[3] = 0;
    for (; i < dst_min; i++, m += 2)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[2];
    }
    for (; i < dst_max; i += 1, m += 2)
    {
        uint8_t* px = src + 3 * ofst[i];
        *(dst++)    = m[0] * px[0] + m[1] * px[3];
        *(dst++)    = m[0] * px[1] + m[1] * px[4];
        *(dst++)    = m[0] * px[2] + m[1] * px[5];
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    ((ufixedpoint16*)(srccn.w))[0] = (src + 3 * ofst[dst_width - 1])[0];
    ((ufixedpoint16*)(srccn.w))[1] = (src + 3 * ofst[dst_width - 1])[1];
    ((ufixedpoint16*)(srccn.w))[2] = (src + 3 * ofst[dst_width - 1])[2];
    for (; i < dst_width; i++)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[2];
    }
}

template <>
void hlineResizeCn<uint8_t, ufixedpoint16, 2, true, 4>(uint8_t* src, int, int* ofst, ufixedpoint16* m, ufixedpoint16* dst, int dst_min, int dst_max, int dst_width)
{
    int i = 0;

    union
    {
        uint64_t q;
        uint16_t w[4];
    } srccn;

    ((ufixedpoint16*)(srccn.w))[0] = src[0];
    ((ufixedpoint16*)(srccn.w))[1] = src[1];
    ((ufixedpoint16*)(srccn.w))[2] = src[2];
    ((ufixedpoint16*)(srccn.w))[3] = src[3];
    for (; i < dst_min; i++, m += 2)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[2];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[3];
    }
    for (; i < dst_max; i += 1, m += 2)
    {
        uint8_t* px = src + 4 * ofst[i];
        *(dst++)    = m[0] * px[0] + m[1] * px[4];
        *(dst++)    = m[0] * px[1] + m[1] * px[5];
        *(dst++)    = m[0] * px[2] + m[1] * px[6];
        *(dst++)    = m[0] * px[3] + m[1] * px[7];
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    ((ufixedpoint16*)(srccn.w))[0] = (src + 4 * ofst[dst_width - 1])[0];
    ((ufixedpoint16*)(srccn.w))[1] = (src + 4 * ofst[dst_width - 1])[1];
    ((ufixedpoint16*)(srccn.w))[2] = (src + 4 * ofst[dst_width - 1])[2];
    ((ufixedpoint16*)(srccn.w))[3] = (src + 4 * ofst[dst_width - 1])[3];
    for (; i < dst_width; i++)
    {
        *(dst++) = ((ufixedpoint16*)(srccn.w))[0];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[1];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[2];
        *(dst++) = ((ufixedpoint16*)(srccn.w))[3];
    }
}

template <>
void hlineResizeCn<uint16_t, ufixedpoint32, 2, true, 1>(uint16_t* src, int, int* ofst, ufixedpoint32* m, ufixedpoint32* dst, int dst_min, int dst_max, int dst_width)
{
    int           i = 0;
    ufixedpoint32 src_0(src[0]);
    for (; i < dst_min; i++, m += 2)
    {
        *(dst++) = src_0;
    }
    for (; i < dst_max; i += 1, m += 2)
    {
        uint16_t* px = src + ofst[i];
        *(dst++)     = m[0] * px[0] + m[1] * px[1];
    }
    // Avoid reading a potentially unset ofst, leading to a random memory read.
    if (i >= dst_width)
    {
        return;
    }
    src_0 = (src + ofst[dst_width - 1])[0];
    for (; i < dst_width; i++)
    {
        *(dst++) = src_0;
    }
}

template <typename ET, typename FT>
void vlineSet(FT* src, ET* dst, int dst_width)
{
    for (int i = 0; i < dst_width; i++)
        dst[i] = src[i];
}

template <>
void vlineSet<uint8_t, ufixedpoint16>(ufixedpoint16* src, uint8_t* dst, int dst_width)
{
    int i = 0;
    for (; i < dst_width; i++)
        *(dst++) = *(src++);
}

template <typename ET, typename FT, int n>
void vlineResize(FT* src, size_t src_step, FT* m, ET* dst, int dst_width)
{
    for (int i = 0; i < dst_width; i++)
    {
        typename FT::WT res = src[i] * m[0];
        for (int k = 1; k < n; k++)
            res = res + src[i + k * src_step] * m[k];
        dst[i] = res;
    }
}

template <>
void vlineResize<uint8_t, ufixedpoint16, 2>(ufixedpoint16* src, size_t src_step, ufixedpoint16* m, uint8_t* dst, int dst_width)
{
    int            i    = 0;
    ufixedpoint16* src1 = src + src_step;
    for (; i < dst_width; i++)
    {
        *(dst++) = (uint8_t)(*(src++) * m[0] + *(src1++) * m[1]);
    }
}

template <typename ET> class interpolationLinear
{
public:
    const static int  len      = 2;
    const static bool needsign = false;

    interpolationLinear(double inv_scale, int srcsize, int dstsize):
        scale(softdouble::one() / softdouble(inv_scale)), maxsize(srcsize), minofst(0), maxofst(dstsize) {}

    void getCoeffs(int val, int* offset, typename fixedtype<ET, needsign>::type* coeffs)
    {
        typedef typename fixedtype<ET, needsign>::type fixedpoint;
        softdouble                                     fval = scale * (softdouble(val) + softdouble(0.5)) - softdouble(0.5);
        int                                            ival = hlFloor(fval);
        if (ival >= 0 && maxsize > 1)
        {
            if (ival < maxsize - 1)
            {
                *offset   = ival;
                coeffs[1] = fval - softdouble(ival);
                coeffs[0] = fixedpoint::one() - coeffs[1];
            }
            else
            {
                *offset = maxsize - 1;
                maxofst = min(maxofst, val);
            }
        }
        else
        {
            minofst = max(minofst, val + 1);
        }
    }

    void getMinMax(int& min, int& max)
    {
        min = minofst;
        max = maxofst;
    }

protected:
    softdouble scale;
    int        maxsize;
    int        minofst, maxofst;
};

template <typename ET, typename FT, int interp_y_len>
class resize_bitExactInvoker:
    public ParallelLoopBody
{
public:
    typedef FT fixedpoint;
    typedef void (*hResizeFunc)(ET* src, int cn, int* ofst, fixedpoint* m, fixedpoint* dst, int dst_min, int dst_max, int dst_width);

    resize_bitExactInvoker(const uchar* _src, size_t _src_step, int _src_width, int _src_height, uchar* _dst, size_t _dst_step, int _dst_width, int _dst_height, int _cn, int* _xoffsets, int* _yoffsets, fixedpoint* _xcoeffs, fixedpoint* _ycoeffs, int _min_x, int _max_x, int _min_y, int _max_y, hResizeFunc _hResize):
        ParallelLoopBody(),
        src(_src),
        src_step(_src_step),
        src_width(_src_width),
        src_height(_src_height),
        dst(_dst),
        dst_step(_dst_step),
        dst_width(_dst_width),
        dst_height(_dst_height),
        cn(_cn),
        xoffsets(_xoffsets),
        yoffsets(_yoffsets),
        xcoeffs(_xcoeffs),
        ycoeffs(_ycoeffs),
        min_x(_min_x),
        max_x(_max_x),
        min_y(_min_y),
        max_y(_max_y),
        hResize(_hResize) {}

    virtual void operator()(const Range& range) const override
    {
        AutoBuffer<fixedpoint> linebuf(interp_y_len * dst_width * cn);
        int                    last_eval     = -interp_y_len;
        int                    evalbuf_start = 0;
        int                    rmin_y        = max(min_y, range.start);
        int                    rmax_y        = min(max_y, range.end);
        if (range.start < min_y)
        {
            last_eval     = 1 - interp_y_len;
            evalbuf_start = 1;
            hResize((ET*)src, cn, xoffsets, xcoeffs, linebuf.data(), min_x, max_x, dst_width);
        }
        int dy = range.start;
        for (; dy < rmin_y; dy++)
            vlineSet<ET, FT>(linebuf.data(), (ET*)(dst + dst_step * dy), dst_width * cn);
        for (; dy < rmax_y; dy++)
        {
            int& iy = yoffsets[dy];

            int i;
            for (i = max(iy, last_eval + interp_y_len); i < min(iy + interp_y_len, src_height); i++, evalbuf_start = (evalbuf_start + 1) % interp_y_len)
                hResize((ET*)(src + i * src_step), cn, xoffsets, xcoeffs, linebuf.data() + evalbuf_start * (dst_width * cn), min_x, max_x, dst_width);
            evalbuf_start = (evalbuf_start + max(iy, src_height - interp_y_len) - max(last_eval, src_height - interp_y_len)) % interp_y_len;
            last_eval     = iy;

            fixedpoint curcoeffs[interp_y_len];
            for (i = 0; i < evalbuf_start; i++)
                curcoeffs[i] = ycoeffs[dy * interp_y_len - evalbuf_start + interp_y_len + i];
            for (; i < interp_y_len; i++)
                curcoeffs[i] = ycoeffs[dy * interp_y_len - evalbuf_start + i];

            vlineResize<ET, FT, interp_y_len>(linebuf.data(), dst_width * cn, curcoeffs, (ET*)(dst + dst_step * dy), dst_width * cn);
        }
        fixedpoint* endline = linebuf.data();
        if (last_eval + interp_y_len > src_height)
            endline += dst_width * cn * ((evalbuf_start + src_height - 1 - last_eval) % interp_y_len);
        else
            hResize((ET*)(src + (src_height - 1) * src_step), cn, xoffsets, xcoeffs, endline, min_x, max_x, dst_width);
        for (; dy < range.end; dy++)
            vlineSet<ET, FT>(endline, (ET*)(dst + dst_step * dy), dst_width * cn);
    }

private:
    const uchar* src;
    size_t       src_step;
    int          src_width, src_height;
    uchar*       dst;
    size_t       dst_step;
    int          dst_width, dst_height, cn;
    int *        xoffsets, *yoffsets;
    fixedpoint * xcoeffs, *ycoeffs;
    int          min_x, max_x, min_y, max_y;
    hResizeFunc  hResize;

    resize_bitExactInvoker(const resize_bitExactInvoker&);
    resize_bitExactInvoker& operator=(const resize_bitExactInvoker&);
};

template <typename ET, typename interpolation>
void resize_bitExact(const uchar* src, size_t src_step, int src_width, int src_height, uchar* dst, size_t dst_step, int dst_width, int dst_height, int cn, double inv_scale_x, double inv_scale_y)
{
    typedef typename fixedtype<ET, interpolation::needsign>::type fixedpoint;
    void (*hResize)(ET* src, int cn, int* ofst, fixedpoint* m, fixedpoint* dst, int dst_min, int dst_max, int dst_width);
    switch (cn)
    {
        case 1  : hResize = src_width > interpolation::len ? hlineResizeCn<ET, fixedpoint, interpolation::len, true, 1> : hlineResizeCn<ET, fixedpoint, interpolation::len, false, 1>; break;
        case 2  : hResize = src_width > interpolation::len ? hlineResizeCn<ET, fixedpoint, interpolation::len, true, 2> : hlineResizeCn<ET, fixedpoint, interpolation::len, false, 2>; break;
        case 3  : hResize = src_width > interpolation::len ? hlineResizeCn<ET, fixedpoint, interpolation::len, true, 3> : hlineResizeCn<ET, fixedpoint, interpolation::len, false, 3>; break;
        case 4  : hResize = src_width > interpolation::len ? hlineResizeCn<ET, fixedpoint, interpolation::len, true, 4> : hlineResizeCn<ET, fixedpoint, interpolation::len, false, 4>; break;
        default : hResize = src_width > interpolation::len ? hlineResize<ET, fixedpoint, interpolation::len, true> : hlineResize<ET, fixedpoint, interpolation::len, false>; break;
    }

    interpolation interp_x(inv_scale_x, src_width, dst_width);
    interpolation interp_y(inv_scale_y, src_height, dst_height);

    AutoBuffer<uchar> buf(dst_width * sizeof(int) + dst_height * sizeof(int) + dst_width * interp_x.len * sizeof(fixedpoint) + dst_height * interp_y.len * sizeof(fixedpoint));
    int*              xoffsets = (int*)buf.data();
    int*              yoffsets = xoffsets + dst_width;
    fixedpoint*       xcoeffs  = (fixedpoint*)(yoffsets + dst_height);
    fixedpoint*       ycoeffs  = xcoeffs + dst_width * interp_x.len;

    int min_x, max_x, min_y, max_y;
    for (int dx = 0; dx < dst_width; dx++)
        interp_x.getCoeffs(dx, xoffsets + dx, xcoeffs + dx * interp_x.len);
    interp_x.getMinMax(min_x, max_x);
    for (int dy = 0; dy < dst_height; dy++)
        interp_y.getCoeffs(dy, yoffsets + dy, ycoeffs + dy * interp_y.len);
    interp_y.getMinMax(min_y, max_y);

    resize_bitExactInvoker<ET, fixedpoint, interpolation::len> invoker(src, src_step, src_width, src_height, dst, dst_step, dst_width, dst_height, cn, xoffsets, yoffsets, xcoeffs, ycoeffs, min_x, max_x, min_y, max_y, hResize);
    Range                                                      range(0, dst_height);
    parallel_for_(range, invoker, dst_width * dst_height / (double)(1 << 16));
}

typedef void (*be_resize_func)(const uchar* src, size_t src_step, int src_width, int src_height, uchar* dst, size_t dst_step, int dst_width, int dst_height, int cn, double inv_scale_x, double inv_scale_y);

}    // namespace

namespace hl
{

/************** interpolation formulas and tables ***************/

const int INTER_RESIZE_COEF_BITS  = 11;
const int INTER_RESIZE_COEF_SCALE = 1 << INTER_RESIZE_COEF_BITS;

inline static void interpolateCubic(float x, float* coeffs)
{
    const float A = -0.75f;

    coeffs[0]     = ((A * (x + 1) - 5 * A) * (x + 1) + 8 * A) * (x + 1) - 4 * A;
    coeffs[1]     = ((A + 2) * x - (A + 3)) * x * x + 1;
    coeffs[2]     = ((A + 2) * (1 - x) - (A + 3)) * (1 - x) * (1 - x) + 1;
    coeffs[3]     = 1.f - coeffs[0] - coeffs[1] - coeffs[2];
}

inline static void interpolateLanczos4(float x, float* coeffs)
{
    const static double s45     = 0.70710678118654752440084436210485;
    const static double cs[][2] = {{1, 0}, {-s45, -s45}, {0, 1}, {s45, -s45}, {-1, 0}, {s45, s45}, {0, -1}, {-s45, s45}};

    float  sum                  = 0;
    double y0 = -(x + 3) * HL_PI * 0.25, s0 = std::sin(y0), c0 = std::cos(y0);
    for (int i = 0; i < 8; i++)
    {
        float y0_ = (x + 3 - i);
        if (fabs(y0_) >= 1e-6f)
        {
            double y  = -y0_ * HL_PI * 0.25;
            coeffs[i] = (float)((cs[i][0] * s0 + cs[i][1] * c0) / (y * y));
        }
        else
        {
            // special handling for 'x' values:
            // - ~0.0: 0 0 0 1 0 0 0 0
            // - ~1.0: 0 0 0 0 1 0 0 0
            coeffs[i] = 1e30f;
        }
        sum += coeffs[i];
    }

    sum = 1.f / sum;
    for (int i = 0; i < 8; i++)
        coeffs[i] *= sum;
}

template <typename ST, typename DT> struct Cast
{
    typedef ST type1;
    typedef DT rtype;

    DT operator()(ST val) const { return saturate_cast<DT>(val); }
};

template <typename ST, typename DT, int bits> struct FixedPtCast
{
    typedef ST type1;
    typedef DT rtype;

    enum
    {
        SHIFT = bits,
        DELTA = 1 << (bits - 1)
    };

    DT operator()(ST val) const { return saturate_cast<DT>((val + DELTA) >> SHIFT); }
};

/****************************************************************************************\
*                                         Resize                                         *
\****************************************************************************************/

class resizeNNInvoker:
    public ParallelLoopBody
{
public:
    resizeNNInvoker(const Mat& _src, Mat& _dst, int* _x_ofs, double _ify):
        ParallelLoopBody(), src(_src), dst(_dst), x_ofs(_x_ofs), ify(_ify)
    {
    }

    virtual void operator()(const Range& range) const override
    {
        Size ssize = src.size(), dsize = dst.size();
        int  y, x, pix_size = (int)src.elemSize();

        for (y = range.start; y < range.end; y++)
        {
            uchar*       D  = dst.data + dst.step * y;
            int          sy = std::min(hlFloor(y * ify), ssize.height - 1);
            const uchar* S  = src.ptr(sy);

            switch (pix_size)
            {
                case 1 :
                    for (x = 0; x <= dsize.width - 2; x += 2)
                    {
                        uchar t0 = S[x_ofs[x]];
                        uchar t1 = S[x_ofs[x + 1]];
                        D[x]     = t0;
                        D[x + 1] = t1;
                    }

                    for (; x < dsize.width; x++)
                        D[x] = S[x_ofs[x]];
                    break;
                case 2 :
                    for (x = 0; x < dsize.width; x++)
                        *(ushort*)(D + x * 2) = *(ushort*)(S + x_ofs[x]);
                    break;
                case 3 :
                    for (x = 0; x < dsize.width; x++, D += 3)
                    {
                        const uchar* _tS = S + x_ofs[x];
                        D[0]             = _tS[0];
                        D[1]             = _tS[1];
                        D[2]             = _tS[2];
                    }
                    break;
                case 4 :
                    for (x = 0; x < dsize.width; x++)
                        *(int*)(D + x * 4) = *(int*)(S + x_ofs[x]);
                    break;
                case 6 :
                    for (x = 0; x < dsize.width; x++, D += 6)
                    {
                        const ushort* _tS = (const ushort*)(S + x_ofs[x]);
                        ushort*       _tD = (ushort*)D;
                        _tD[0]            = _tS[0];
                        _tD[1]            = _tS[1];
                        _tD[2]            = _tS[2];
                    }
                    break;
                case 8 :
                    for (x = 0; x < dsize.width; x++, D += 8)
                    {
                        const int* _tS = (const int*)(S + x_ofs[x]);
                        int*       _tD = (int*)D;
                        _tD[0]         = _tS[0];
                        _tD[1]         = _tS[1];
                    }
                    break;
                case 12 :
                    for (x = 0; x < dsize.width; x++, D += 12)
                    {
                        const int* _tS = (const int*)(S + x_ofs[x]);
                        int*       _tD = (int*)D;
                        _tD[0]         = _tS[0];
                        _tD[1]         = _tS[1];
                        _tD[2]         = _tS[2];
                    }
                    break;
                default :
                    for (x = 0; x < dsize.width; x++, D += pix_size)
                    {
                        const uchar* _tS = S + x_ofs[x];
                        for (int k = 0; k < pix_size; k++)
                            D[k] = _tS[k];
                    }
            }
        }
    }

private:
    const Mat& src;
    Mat&       dst;
    int*       x_ofs;
    double     ify;

    resizeNNInvoker(const resizeNNInvoker&);
    resizeNNInvoker& operator=(const resizeNNInvoker&);
};

static void resizeNN(const Mat& src, Mat& dst, double fx, double fy)
{
    Size            ssize = src.size(), dsize = dst.size();
    AutoBuffer<int> _x_ofs(dsize.width);
    int*            x_ofs    = _x_ofs.data();
    int             pix_size = (int)src.elemSize();
    double          ifx = 1. / fx, ify = 1. / fy;
    int             x;

    for (x = 0; x < dsize.width; x++)
    {
        int sx   = hlFloor(x * ifx);
        x_ofs[x] = std::min(sx, ssize.width - 1) * pix_size;
    }

    Range           range(0, dsize.height);
    resizeNNInvoker invoker(src, dst, x_ofs, ify);
    parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

class resizeNN_bitexactInvoker: public ParallelLoopBody
{
public:
    resizeNN_bitexactInvoker(const Mat& _src, Mat& _dst, int* _x_ofse, int _ify, int _ify0):
        src(_src), dst(_dst), x_ofse(_x_ofse), ify(_ify), ify0(_ify0) {}

    virtual void operator()(const Range& range) const override
    {
        Size ssize = src.size(), dsize = dst.size();
        int  pix_size = (int)src.elemSize();
        for (int y = range.start; y < range.end; y++)
        {
            uchar*       D   = dst.ptr(y);
            int          _sy = (ify * y + ify0) >> 16;
            int          sy  = std::min(_sy, ssize.height - 1);
            const uchar* S   = src.ptr(sy);

            int x            = 0;
            switch (pix_size)
            {
                case 1 :
                    for (; x < dsize.width; x++)
                        D[x] = S[x_ofse[x]];
                    break;
                case 2 :
                    for (; x < dsize.width; x++)
                        *((ushort*)D + x) = *((ushort*)S + x_ofse[x]);
                    break;
                case 3 :
                    for (; x < dsize.width; x++, D += 3)
                    {
                        const uchar* _tS = S + x_ofse[x] * 3;
                        D[0]             = _tS[0];
                        D[1]             = _tS[1];
                        D[2]             = _tS[2];
                    }
                    break;
                case 4 :
                    for (; x < dsize.width; x++)
                        *((uint32_t*)D + x) = *((uint32_t*)S + x_ofse[x]);
                    break;
                case 6 :
                    for (; x < dsize.width; x++, D += 6)
                    {
                        const ushort* _tS = (const ushort*)(S + x_ofse[x] * 6);
                        ushort*       _tD = (ushort*)D;
                        _tD[0]            = _tS[0];
                        _tD[1]            = _tS[1];
                        _tD[2]            = _tS[2];
                    }
                    break;
                case 8 :
                    for (; x < dsize.width; x++)
                        *((uint64_t*)D + x) = *((uint64_t*)S + x_ofse[x]);
                    break;
                case 12 :
                    for (; x < dsize.width; x++, D += 12)
                    {
                        const int* _tS = (const int*)(S + x_ofse[x] * 12);
                        int*       _tD = (int*)D;
                        _tD[0]         = _tS[0];
                        _tD[1]         = _tS[1];
                        _tD[2]         = _tS[2];
                    }
                    break;
                default :
                    for (x = 0; x < dsize.width; x++, D += pix_size)
                    {
                        const uchar* _tS = S + x_ofse[x] * pix_size;
                        for (int k = 0; k < pix_size; k++)
                            D[k] = _tS[k];
                    }
            }
        }
    }

private:
    const Mat& src;
    Mat&       dst;
    int*       x_ofse;
    const int  ify;
    const int  ify0;
};

static void resizeNN_bitexact(const Mat& src, Mat& dst, double /*fx*/, double /*fy*/)
{
    Size ssize = src.size(), dsize = dst.size();
    int  ifx  = ((ssize.width << 16) + dsize.width / 2) / dsize.width;    // 16bit fixed-point arithmetic
    int  ifx0 = ifx / 2 - ssize.width % 2;                                // This method uses center pixel coordinate as Pillow and scikit-images do.
    int  ify  = ((ssize.height << 16) + dsize.height / 2) / dsize.height;
    int  ify0 = ify / 2 - ssize.height % 2;

    hl::utils::BufferArea area;
    int*                  x_ofse = 0;
    area.allocate(x_ofse, dsize.width, HL_SIMD_WIDTH);
    area.commit();

    for (int x = 0; x < dsize.width; x++)
    {
        int sx    = (ifx * x + ifx0) >> 16;
        x_ofse[x] = std::min(sx, ssize.width - 1);    // offset in element (not byte)
    }
    Range                    range(0, dsize.height);
    resizeNN_bitexactInvoker invoker(src, dst, x_ofse, ify, ify0);
    parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

struct VResizeNoVec
{
    template <typename WT, typename T, typename BT>
    int operator()(const WT**, T*, const BT*, int) const
    {
        return 0;
    }
};

struct HResizeNoVec
{
    template <typename T, typename WT, typename AT>
    inline int operator()(const T**, WT**, int, const int*, const AT*, int, int, int, int, int) const
    {
        return 0;
    }
};

typedef VResizeNoVec VResizeLinearVec_32s8u;
typedef VResizeNoVec VResizeLinearVec_32f16u;
typedef VResizeNoVec VResizeLinearVec_32f16s;
typedef VResizeNoVec VResizeLinearVec_32f;

typedef VResizeNoVec VResizeCubicVec_32s8u;
typedef VResizeNoVec VResizeCubicVec_32f16u;
typedef VResizeNoVec VResizeCubicVec_32f16s;
typedef VResizeNoVec VResizeCubicVec_32f;

typedef VResizeNoVec VResizeLanczos4Vec_32f16u;
typedef VResizeNoVec VResizeLanczos4Vec_32f16s;
typedef VResizeNoVec VResizeLanczos4Vec_32f;

typedef HResizeNoVec HResizeLinearVec_8u32s;
typedef HResizeNoVec HResizeLinearVec_16u32f;
typedef HResizeNoVec HResizeLinearVec_16s32f;
typedef HResizeNoVec HResizeLinearVec_32f;
typedef HResizeNoVec HResizeLinearVec_64f;

template <typename T, typename WT, typename AT, int ONE, class VecOp>
struct HResizeLinear
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count, const int* xofs, const AT* alpha, int swidth, int dwidth, int cn, int xmin, int xmax) const
    {
        int   dx, k;
        VecOp vecOp;

        int dx0 = vecOp(src, dst, count, xofs, alpha, swidth, dwidth, cn, xmin, xmax);

        for (k = 0; k <= count - 2; k += 2)
        {
            const T *S0 = src[k], *S1 = src[k + 1];
            WT *     D0 = dst[k], *D1 = dst[k + 1];
            for (dx = dx0; dx < xmax; dx++)
            {
                int sx = xofs[dx];
                WT  a0 = alpha[dx * 2], a1 = alpha[dx * 2 + 1];
                WT  t0 = S0[sx] * a0 + S0[sx + cn] * a1;
                WT  t1 = S1[sx] * a0 + S1[sx + cn] * a1;
                D0[dx] = t0;
                D1[dx] = t1;
            }

            for (; dx < dwidth; dx++)
            {
                int sx = xofs[dx];
                D0[dx] = WT(S0[sx] * ONE);
                D1[dx] = WT(S1[sx] * ONE);
            }
        }

        for (; k < count; k++)
        {
            const T* S = src[k];
            WT*      D = dst[k];
            for (dx = dx0; dx < xmax; dx++)
            {
                int sx = xofs[dx];
                D[dx]  = S[sx] * alpha[dx * 2] + S[sx + cn] * alpha[dx * 2 + 1];
            }

            for (; dx < dwidth; dx++)
                D[dx] = WT(S[xofs[dx]] * ONE);
        }
    }
};

template <typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeLinear
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width) const
    {
        WT        b0 = beta[0], b1 = beta[1];
        const WT *S0 = src[0], *S1 = src[1];
        CastOp    castOp;
        VecOp     vecOp;

        int x = vecOp(src, dst, beta, width);
        for (; x <= width - 4; x += 4)
        {
            WT t0, t1;
            t0         = S0[x] * b0 + S1[x] * b1;
            t1         = S0[x + 1] * b0 + S1[x + 1] * b1;
            dst[x]     = castOp(t0);
            dst[x + 1] = castOp(t1);
            t0         = S0[x + 2] * b0 + S1[x + 2] * b1;
            t1         = S0[x + 3] * b0 + S1[x + 3] * b1;
            dst[x + 2] = castOp(t0);
            dst[x + 3] = castOp(t1);
        }
        for (; x < width; x++)
            dst[x] = castOp(S0[x] * b0 + S1[x] * b1);
    }
};

template <>
struct VResizeLinear<uchar, int, short, FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS * 2>, VResizeLinearVec_32s8u>
{
    typedef uchar value_type;
    typedef int   buf_type;
    typedef short alpha_type;

    void operator()(const buf_type** src, value_type* dst, const alpha_type* beta, int width) const
    {
        alpha_type             b0 = beta[0], b1 = beta[1];
        const buf_type *       S0 = src[0], *S1 = src[1];
        VResizeLinearVec_32s8u vecOp;

        int x = vecOp(src, dst, beta, width);
        for (; x <= width - 4; x += 4)
        {
            dst[x + 0] = uchar((((b0 * (S0[x + 0] >> 4)) >> 16) + ((b1 * (S1[x + 0] >> 4)) >> 16) + 2) >> 2);
            dst[x + 1] = uchar((((b0 * (S0[x + 1] >> 4)) >> 16) + ((b1 * (S1[x + 1] >> 4)) >> 16) + 2) >> 2);
            dst[x + 2] = uchar((((b0 * (S0[x + 2] >> 4)) >> 16) + ((b1 * (S1[x + 2] >> 4)) >> 16) + 2) >> 2);
            dst[x + 3] = uchar((((b0 * (S0[x + 3] >> 4)) >> 16) + ((b1 * (S1[x + 3] >> 4)) >> 16) + 2) >> 2);
        }
        for (; x < width; x++)
            dst[x] = uchar((((b0 * (S0[x] >> 4)) >> 16) + ((b1 * (S1[x] >> 4)) >> 16) + 2) >> 2);
    }
};

template <typename T, typename WT, typename AT>
struct HResizeCubic
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count, const int* xofs, const AT* alpha, int swidth, int dwidth, int cn, int xmin, int xmax) const
    {
        for (int k = 0; k < count; k++)
        {
            const T* S  = src[k];
            WT*      D  = dst[k];
            int      dx = 0, limit = xmin;
            for (;;)
            {
                for (; dx < limit; dx++, alpha += 4)
                {
                    int j, sx = xofs[dx] - cn;
                    WT  v = 0;
                    for (j = 0; j < 4; j++)
                    {
                        int sxj = sx + j * cn;
                        if ((unsigned)sxj >= (unsigned)swidth)
                        {
                            while (sxj < 0)
                                sxj += cn;
                            while (sxj >= swidth)
                                sxj -= cn;
                        }
                        v += S[sxj] * alpha[j];
                    }
                    D[dx] = v;
                }
                if (limit == dwidth)
                    break;
                for (; dx < xmax; dx++, alpha += 4)
                {
                    int sx = xofs[dx];
                    D[dx]  = S[sx - cn] * alpha[0] + S[sx] * alpha[1] + S[sx + cn] * alpha[2] + S[sx + cn * 2] * alpha[3];
                }
                limit = dwidth;
            }
            alpha -= dwidth * 4;
        }
    }
};

template <typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeCubic
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width) const
    {
        WT        b0 = beta[0], b1 = beta[1], b2 = beta[2], b3 = beta[3];
        const WT *S0 = src[0], *S1 = src[1], *S2 = src[2], *S3 = src[3];
        CastOp    castOp;
        VecOp     vecOp;

        int x = vecOp(src, dst, beta, width);
        for (; x < width; x++)
            dst[x] = castOp(S0[x] * b0 + S1[x] * b1 + S2[x] * b2 + S3[x] * b3);
    }
};

template <typename T, typename WT, typename AT>
struct HResizeLanczos4
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const T** src, WT** dst, int count, const int* xofs, const AT* alpha, int swidth, int dwidth, int cn, int xmin, int xmax) const
    {
        for (int k = 0; k < count; k++)
        {
            const T* S  = src[k];
            WT*      D  = dst[k];
            int      dx = 0, limit = xmin;
            for (;;)
            {
                for (; dx < limit; dx++, alpha += 8)
                {
                    int j, sx = xofs[dx] - cn * 3;
                    WT  v = 0;
                    for (j = 0; j < 8; j++)
                    {
                        int sxj = sx + j * cn;
                        if ((unsigned)sxj >= (unsigned)swidth)
                        {
                            while (sxj < 0)
                                sxj += cn;
                            while (sxj >= swidth)
                                sxj -= cn;
                        }
                        v += S[sxj] * alpha[j];
                    }
                    D[dx] = v;
                }
                if (limit == dwidth)
                    break;
                for (; dx < xmax; dx++, alpha += 8)
                {
                    int sx = xofs[dx];
                    D[dx]  = S[sx - cn * 3] * alpha[0] + S[sx - cn * 2] * alpha[1] + S[sx - cn] * alpha[2] + S[sx] * alpha[3] + S[sx + cn] * alpha[4] + S[sx + cn * 2] * alpha[5] + S[sx + cn * 3] * alpha[6] + S[sx + cn * 4] * alpha[7];
                }
                limit = dwidth;
            }
            alpha -= dwidth * 8;
        }
    }
};

template <typename T, typename WT, typename AT, class CastOp, class VecOp>
struct VResizeLanczos4
{
    typedef T  value_type;
    typedef WT buf_type;
    typedef AT alpha_type;

    void operator()(const WT** src, T* dst, const AT* beta, int width) const
    {
        CastOp castOp;
        VecOp  vecOp;
        int    x = vecOp(src, dst, beta, width);
        for (; x <= width - 4; x += 4)
        {
            WT        b  = beta[0];
            const WT* S  = src[0];
            WT        s0 = S[x] * b, s1 = S[x + 1] * b, s2 = S[x + 2] * b, s3 = S[x + 3] * b;

            for (int k = 1; k < 8; k++)
            {
                b   = beta[k];
                S   = src[k];
                s0 += S[x] * b;
                s1 += S[x + 1] * b;
                s2 += S[x + 2] * b;
                s3 += S[x + 3] * b;
            }

            dst[x]     = castOp(s0);
            dst[x + 1] = castOp(s1);
            dst[x + 2] = castOp(s2);
            dst[x + 3] = castOp(s3);
        }
        for (; x < width; x++)
        {
            dst[x] = castOp(src[0][x] * beta[0] + src[1][x] * beta[1] + src[2][x] * beta[2] + src[3][x] * beta[3] + src[4][x] * beta[4] + src[5][x] * beta[5] + src[6][x] * beta[6] + src[7][x] * beta[7]);
        }
    }
};

inline static int clip(int x, int a, int b)
{
    return x >= a ? (x < b ? x : b - 1) : a;
}

const static int MAX_ESIZE = 16;

template <typename HResize, typename VResize>
class resizeGeneric_Invoker:
    public ParallelLoopBody
{
public:
    typedef typename HResize::value_type T;
    typedef typename HResize::buf_type   WT;
    typedef typename HResize::alpha_type AT;

    resizeGeneric_Invoker(const Mat& _src, Mat& _dst, const int* _xofs, const int* _yofs, const AT* _alpha, const AT* __beta, const Size& _ssize, const Size& _dsize, int _ksize, int _xmin, int _xmax):
        ParallelLoopBody(), src(_src), dst(_dst), xofs(_xofs), yofs(_yofs), alpha(_alpha), _beta(__beta), ssize(_ssize), dsize(_dsize), ksize(_ksize), xmin(_xmin), xmax(_xmax)
    {
        HL_Assert(ksize <= MAX_ESIZE);
    }

    virtual void operator()(const Range& range) const override
    {
        int     dy, cn = src.channels();
        HResize hresize;
        VResize vresize;

        int            bufstep = (int)alignSize(dsize.width, 16);
        AutoBuffer<WT> _buffer(bufstep * ksize);
        const T*       srows[MAX_ESIZE] = {0};
        WT*            rows[MAX_ESIZE]  = {0};
        int            prev_sy[MAX_ESIZE];

        for (int k = 0; k < ksize; k++)
        {
            prev_sy[k] = -1;
            rows[k]    = _buffer.data() + bufstep * k;
        }

        const AT* beta = _beta + ksize * range.start;

        for (dy = range.start; dy < range.end; dy++, beta += ksize)
        {
            int sy0 = yofs[dy], k0 = ksize, k1 = 0, ksize2 = ksize / 2;

            for (int k = 0; k < ksize; k++)
            {
                int sy = clip(sy0 - ksize2 + 1 + k, 0, ssize.height);
                for (k1 = std::max(k1, k); k1 < ksize; k1++)
                {
                    if (k1 < MAX_ESIZE && sy == prev_sy[k1])    // if the sy-th row has been computed already, reuse it.
                    {
                        if (k1 > k)
                            memcpy(rows[k], rows[k1], bufstep * sizeof(rows[0][0]));
                        break;
                    }
                }
                if (k1 == ksize)
                    k0 = std::min(k0, k);    // remember the first row that needs to be computed
                srows[k]   = src.template ptr<T>(sy);
                prev_sy[k] = sy;
            }

            if (k0 < ksize)
                hresize((const T**)(srows + k0), (WT**)(rows + k0), ksize - k0, xofs, (const AT*)(alpha), ssize.width, dsize.width, cn, xmin, xmax);
            vresize((const WT**)rows, (T*)(dst.data + dst.step * dy), beta, dsize.width);
        }
    }

private:
    Mat        src;
    Mat        dst;
    const int *xofs, *yofs;
    const AT * alpha, *_beta;
    Size       ssize, dsize;
    const int  ksize, xmin, xmax;

    resizeGeneric_Invoker& operator=(const resizeGeneric_Invoker&);
};

template <class HResize, class VResize>
static void resizeGeneric_(const Mat& src, Mat& dst, const int* xofs, const void* _alpha, const int* yofs, const void* _beta, int xmin, int xmax, int ksize)
{
    typedef typename HResize::alpha_type AT;

    const AT* beta  = (const AT*)_beta;
    Size      ssize = src.size(), dsize = dst.size();
    int       cn  = src.channels();
    ssize.width  *= cn;
    dsize.width  *= cn;
    xmin         *= cn;
    xmax         *= cn;
    // image resize is a separable operation. In case of not too strong

    Range                                   range(0, dsize.height);
    resizeGeneric_Invoker<HResize, VResize> invoker(src, dst, xofs, yofs, (const AT*)_alpha, beta, ssize, dsize, ksize, xmin, xmax);
    parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

template <typename T, typename WT>
struct ResizeAreaFastNoVec
{
    ResizeAreaFastNoVec(int, int) {}

    ResizeAreaFastNoVec(int, int, int, int) {}

    int operator()(const T*, T*, int) const
    {
        return 0;
    }
};

typedef ResizeAreaFastNoVec<uchar, uchar>   ResizeAreaFastVec_SIMD_8u;
typedef ResizeAreaFastNoVec<ushort, ushort> ResizeAreaFastVec_SIMD_16u;
typedef ResizeAreaFastNoVec<short, short>   ResizeAreaFastVec_SIMD_16s;
typedef ResizeAreaFastNoVec<float, float>   ResizeAreaFastVec_SIMD_32f;

template <typename T, typename SIMDVecOp>
struct ResizeAreaFastVec
{
    ResizeAreaFastVec(int _scale_x, int _scale_y, int _cn, int _step):
        scale_x(_scale_x), scale_y(_scale_y), cn(_cn), step(_step), vecOp(_cn, _step)
    {
        fast_mode = scale_x == 2 && scale_y == 2 && (cn == 1 || cn == 3 || cn == 4);
    }

    int operator()(const T* S, T* D, int w) const
    {
        if (!fast_mode)
            return 0;

        const T* nextS = (const T*)((const uchar*)S + step);
        int      dx    = vecOp(S, D, w);

        if (cn == 1)
            for (; dx < w; ++dx)
            {
                int index = dx * 2;
                D[dx]     = (T)((S[index] + S[index + 1] + nextS[index] + nextS[index + 1] + 2) >> 2);
            }
        else if (cn == 3)
            for (; dx < w; dx += 3)
            {
                int index = dx * 2;
                D[dx]     = (T)((S[index] + S[index + 3] + nextS[index] + nextS[index + 3] + 2) >> 2);
                D[dx + 1] = (T)((S[index + 1] + S[index + 4] + nextS[index + 1] + nextS[index + 4] + 2) >> 2);
                D[dx + 2] = (T)((S[index + 2] + S[index + 5] + nextS[index + 2] + nextS[index + 5] + 2) >> 2);
            }
        else
        {
            HL_Assert(cn == 4);
            for (; dx < w; dx += 4)
            {
                int index = dx * 2;
                D[dx]     = (T)((S[index] + S[index + 4] + nextS[index] + nextS[index + 4] + 2) >> 2);
                D[dx + 1] = (T)((S[index + 1] + S[index + 5] + nextS[index + 1] + nextS[index + 5] + 2) >> 2);
                D[dx + 2] = (T)((S[index + 2] + S[index + 6] + nextS[index + 2] + nextS[index + 6] + 2) >> 2);
                D[dx + 3] = (T)((S[index + 3] + S[index + 7] + nextS[index + 3] + nextS[index + 7] + 2) >> 2);
            }
        }

        return dx;
    }

private:
    int       scale_x, scale_y;
    int       cn;
    bool      fast_mode;
    int       step;
    SIMDVecOp vecOp;
};

template <typename T, typename WT, typename VecOp>
class resizeAreaFast_Invoker:
    public ParallelLoopBody
{
public:
    resizeAreaFast_Invoker(const Mat& _src, Mat& _dst, int _scale_x, int _scale_y, const int* _ofs, const int* _xofs):
        ParallelLoopBody(), src(_src), dst(_dst), scale_x(_scale_x), scale_y(_scale_y), ofs(_ofs), xofs(_xofs)
    {
    }

    virtual void operator()(const Range& range) const override
    {
        Size  ssize = src.size(), dsize = dst.size();
        int   cn       = src.channels();
        int   area     = scale_x * scale_y;
        float scale    = 1.f / (area);
        int   dwidth1  = (ssize.width / scale_x) * cn;
        dsize.width   *= cn;
        ssize.width   *= cn;
        int dy, dx, k = 0;

        VecOp vop(scale_x, scale_y, src.channels(), (int)src.step /*, area_ofs*/);

        for (dy = range.start; dy < range.end; dy++)
        {
            T*  D   = (T*)(dst.data + dst.step * dy);
            int sy0 = dy * scale_y;
            int w   = sy0 + scale_y <= ssize.height ? dwidth1 : 0;

            if (sy0 >= ssize.height)
            {
                for (dx = 0; dx < dsize.width; dx++)
                    D[dx] = 0;
                continue;
            }

            dx = vop(src.template ptr<T>(sy0), D, w);
            for (; dx < w; dx++)
            {
                const T* S   = src.template ptr<T>(sy0) + xofs[dx];
                WT       sum = 0;
                k            = 0;
                for (; k <= area - 4; k += 4)
                    sum += S[ofs[k]] + S[ofs[k + 1]] + S[ofs[k + 2]] + S[ofs[k + 3]];
                for (; k < area; k++)
                    sum += S[ofs[k]];

                D[dx] = saturate_cast<T>(sum * scale);
            }

            for (; dx < dsize.width; dx++)
            {
                WT  sum   = 0;
                int count = 0, sx0 = xofs[dx];
                if (sx0 >= ssize.width)
                    D[dx] = 0;

                for (int sy = 0; sy < scale_y; sy++)
                {
                    if (sy0 + sy >= ssize.height)
                        break;
                    const T* S = src.template ptr<T>(sy0 + sy) + sx0;
                    for (int sx = 0; sx < scale_x * cn; sx += cn)
                    {
                        if (sx0 + sx >= ssize.width)
                            break;
                        sum += S[sx];
                        count++;
                    }
                }

                D[dx] = saturate_cast<T>((float)sum / count);
            }
        }
    }

private:
    Mat        src;
    Mat        dst;
    int        scale_x, scale_y;
    const int *ofs, *xofs;
};

template <typename T, typename WT, typename VecOp>
static void resizeAreaFast_(const Mat& src, Mat& dst, const int* ofs, const int* xofs, int scale_x, int scale_y)
{
    Range                                range(0, dst.rows);
    resizeAreaFast_Invoker<T, WT, VecOp> invoker(src, dst, scale_x, scale_y, ofs, xofs);
    parallel_for_(range, invoker, dst.total() / (double)(1 << 16));
}

struct DecimateAlpha
{
    int   si, di;
    float alpha;
};

namespace inter_area
{


inline void mul(const double* buf, int width, double beta, double* sum)
{
    for (int dx = 0; dx < width; ++dx)
    {
        sum[dx] = beta * buf[dx];
    }
}

inline void muladd(const double* buf, int width, double beta, double* sum)
{
    for (int dx = 0; dx < width; ++dx)
    {
        sum[dx] += beta * buf[dx];
    }
}

template <typename T, typename WT>
inline void saturate_store(const WT* sum, int width, T* D)
{
    int dx = 0;
    for (; dx < width; ++dx)
    {
        D[dx] = saturate_cast<T>(sum[dx]);
    }
}

// Optimization when T == WT.
template <typename WT>
inline void saturate_store(const WT* sum, int width, WT* D)
{
    std::copy(sum, sum + width, D);
}

template <typename WT>
inline void mul(const WT* buf, int width, WT beta, WT* sum)
{
    for (int dx = 0; dx < width; ++dx)
    {
        sum[dx] = beta * buf[dx];
    }
}

template <typename WT>
inline void muladd(const WT* buf, int width, WT beta, WT* sum)
{
    for (int dx = 0; dx < width; ++dx)
    {
        sum[dx] += beta * buf[dx];
    }
}

}    // namespace inter_area

template <typename T, typename WT> class ResizeArea_Invoker:
    public ParallelLoopBody
{
public:
    ResizeArea_Invoker(const Mat& _src, Mat& _dst, const DecimateAlpha* _xtab, int _xtab_size, const DecimateAlpha* _ytab, int _ytab_size, const int* _tabofs)
    {
        src        = &_src;
        dst        = &_dst;
        xtab0      = _xtab;
        xtab_size0 = _xtab_size;
        ytab       = _ytab;
        ytab_size  = _ytab_size;
        tabofs     = _tabofs;
    }

    virtual void operator()(const Range& range) const override
    {
        Size dsize   = dst->size();
        int  cn      = dst->channels();
        dsize.width *= cn;
        AutoBuffer<WT>       _buffer(dsize.width * 2);
        const DecimateAlpha* xtab      = xtab0;
        int                  xtab_size = xtab_size0;
        WT *                 buf = _buffer.data(), *sum = buf + dsize.width;
        int                  j_start = tabofs[range.start], j_end = tabofs[range.end], j, k, dx, prev_dy = ytab[j_start].di;

        for (dx = 0; dx < dsize.width; dx++)
            sum[dx] = (WT)0;

        for (j = j_start; j < j_end; j++)
        {
            WT  beta = ytab[j].alpha;
            int dy   = ytab[j].di;
            int sy   = ytab[j].si;

            {
                const T* S = src->template ptr<T>(sy);
                for (dx = 0; dx < dsize.width; dx++)
                    buf[dx] = (WT)0;

                if (cn == 1)
                    for (k = 0; k < xtab_size; k++)
                    {
                        int dxn    = xtab[k].di;
                        WT  alpha  = xtab[k].alpha;
                        buf[dxn]  += S[xtab[k].si] * alpha;
                    }
                else if (cn == 2)
                    for (k = 0; k < xtab_size; k++)
                    {
                        int sxn      = xtab[k].si;
                        int dxn      = xtab[k].di;
                        WT  alpha    = xtab[k].alpha;
                        WT  t0       = buf[dxn] + S[sxn] * alpha;
                        WT  t1       = buf[dxn + 1] + S[sxn + 1] * alpha;
                        buf[dxn]     = t0;
                        buf[dxn + 1] = t1;
                    }
                else if (cn == 3)
                    for (k = 0; k < xtab_size; k++)
                    {
                        int sxn      = xtab[k].si;
                        int dxn      = xtab[k].di;
                        WT  alpha    = xtab[k].alpha;
                        WT  t0       = buf[dxn] + S[sxn] * alpha;
                        WT  t1       = buf[dxn + 1] + S[sxn + 1] * alpha;
                        WT  t2       = buf[dxn + 2] + S[sxn + 2] * alpha;
                        buf[dxn]     = t0;
                        buf[dxn + 1] = t1;
                        buf[dxn + 2] = t2;
                    }
                else if (cn == 4)
                {
                    for (k = 0; k < xtab_size; k++)
                    {
                        int sxn      = xtab[k].si;
                        int dxn      = xtab[k].di;
                        WT  alpha    = xtab[k].alpha;
                        WT  t0       = buf[dxn] + S[sxn] * alpha;
                        WT  t1       = buf[dxn + 1] + S[sxn + 1] * alpha;
                        buf[dxn]     = t0;
                        buf[dxn + 1] = t1;
                        t0           = buf[dxn + 2] + S[sxn + 2] * alpha;
                        t1           = buf[dxn + 3] + S[sxn + 3] * alpha;
                        buf[dxn + 2] = t0;
                        buf[dxn + 3] = t1;
                    }
                }
                else
                {
                    for (k = 0; k < xtab_size; k++)
                    {
                        int sxn   = xtab[k].si;
                        int dxn   = xtab[k].di;
                        WT  alpha = xtab[k].alpha;
                        for (int c = 0; c < cn; c++)
                            buf[dxn + c] += S[sxn + c] * alpha;
                    }
                }
            }

            if (dy != prev_dy)
            {
                inter_area::saturate_store(sum, dsize.width, dst->template ptr<T>(prev_dy));
                inter_area::mul(buf, dsize.width, beta, sum);
                prev_dy = dy;
            }
            else
            {
                inter_area::muladd(buf, dsize.width, beta, sum);
            }
        }

        inter_area::saturate_store(sum, dsize.width, dst->template ptr<T>(prev_dy));
    }

private:
    const Mat*           src;
    Mat*                 dst;
    const DecimateAlpha* xtab0;
    const DecimateAlpha* ytab;
    int                  xtab_size0, ytab_size;
    const int*           tabofs;
};

template <typename T, typename WT>
static void resizeArea_(const Mat& src, Mat& dst, const DecimateAlpha* xtab, int xtab_size, const DecimateAlpha* ytab, int ytab_size, const int* tabofs)
{
    parallel_for_(Range(0, dst.rows), ResizeArea_Invoker<T, WT>(src, dst, xtab, xtab_size, ytab, ytab_size, tabofs), dst.total() / ((double)(1 << 16)));
}

typedef void (*ResizeFunc)(const Mat& src, Mat& dst, const int* xofs, const void* alpha, const int* yofs, const void* beta, int xmin, int xmax, int ksize);
typedef void (*ResizeAreaFastFunc)(const Mat& src, Mat& dst, const int* ofs, const int* xofs, int scale_x, int scale_y);
typedef void (*ResizeAreaFunc)(const Mat& src, Mat& dst, const DecimateAlpha* xtab, int xtab_size, const DecimateAlpha* ytab, int ytab_size, const int* yofs);

static int computeResizeAreaTab(int ssize, int dsize, int cn, double scale, DecimateAlpha* tab)
{
    int k = 0;
    for (int dx = 0; dx < dsize; dx++)
    {
        double fsx1      = dx * scale;
        double fsx2      = fsx1 + scale;
        double cellWidth = std::min(scale, ssize - fsx1);

        int sx1 = hlCeil(fsx1), sx2 = hlFloor(fsx2);

        sx2 = std::min(sx2, ssize - 1);
        sx1 = std::min(sx1, sx2);

        if (sx1 - fsx1 > 1e-3)
        {
            HL_Assert(k < ssize * 2);
            tab[k].di      = dx * cn;
            tab[k].si      = (sx1 - 1) * cn;
            tab[k++].alpha = (float)((sx1 - fsx1) / cellWidth);
        }

        for (int sx = sx1; sx < sx2; sx++)
        {
            HL_Assert(k < ssize * 2);
            tab[k].di      = dx * cn;
            tab[k].si      = sx * cn;
            tab[k++].alpha = float(1.0 / cellWidth);
        }

        if (fsx2 - sx2 > 1e-3)
        {
            HL_Assert(k < ssize * 2);
            tab[k].di      = dx * cn;
            tab[k].si      = sx2 * cn;
            tab[k++].alpha = (float)(std::min(std::min(fsx2 - sx2, 1.), cellWidth) / cellWidth);
        }
    }
    return k;
}

namespace hal
{

void resize(int src_type, const uchar* src_data, size_t src_step, int src_width, int src_height, uchar* dst_data, size_t dst_step, int dst_width, int dst_height, double inv_scale_x, double inv_scale_y, int interpolation)
{
    HL_Assert((dst_width > 0 && dst_height > 0) || (inv_scale_x > 0 && inv_scale_y > 0));
    if (inv_scale_x < DBL_EPSILON || inv_scale_y < DBL_EPSILON)
    {
        inv_scale_x = static_cast<double>(dst_width) / src_width;
        inv_scale_y = static_cast<double>(dst_height) / src_height;
    }

    int  depth = HL_MAT_DEPTH(src_type), cn = HL_MAT_CN(src_type);
    Size dsize = Size(saturate_cast<int>(src_width * inv_scale_x), saturate_cast<int>(src_height * inv_scale_y));
    HL_Assert(!dsize.empty());

    static ResizeFunc linear_tab[] = {
        resizeGeneric_<HResizeLinear<uchar, int, short, INTER_RESIZE_COEF_SCALE, HResizeLinearVec_8u32s>,
                       VResizeLinear<uchar, int, short, FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS * 2>, VResizeLinearVec_32s8u>>,
        0,
        resizeGeneric_<HResizeLinear<ushort, float, float, 1, HResizeLinearVec_16u32f>,
                       VResizeLinear<ushort, float, float, Cast<float, ushort>, VResizeLinearVec_32f16u>>,
        resizeGeneric_<HResizeLinear<short, float, float, 1, HResizeLinearVec_16s32f>,
                       VResizeLinear<short, float, float, Cast<float, short>, VResizeLinearVec_32f16s>>,
        0,
        resizeGeneric_<HResizeLinear<float, float, float, 1, HResizeLinearVec_32f>,
                       VResizeLinear<float, float, float, Cast<float, float>, VResizeLinearVec_32f>>,
        resizeGeneric_<HResizeLinear<double, double, float, 1, HResizeNoVec>,
                       VResizeLinear<double, double, float, Cast<double, double>, VResizeNoVec>>,
        0};

    static ResizeFunc cubic_tab[] = {
        resizeGeneric_<
            HResizeCubic<uchar, int, short>,
            VResizeCubic<uchar, int, short, FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS * 2>, VResizeCubicVec_32s8u>>,
        0,
        resizeGeneric_<
            HResizeCubic<ushort, float, float>,
            VResizeCubic<ushort, float, float, Cast<float, ushort>, VResizeCubicVec_32f16u>>,
        resizeGeneric_<
            HResizeCubic<short, float, float>,
            VResizeCubic<short, float, float, Cast<float, short>, VResizeCubicVec_32f16s>>,
        0,
        resizeGeneric_<
            HResizeCubic<float, float, float>,
            VResizeCubic<float, float, float, Cast<float, float>, VResizeCubicVec_32f>>,
        resizeGeneric_<
            HResizeCubic<double, double, float>,
            VResizeCubic<double, double, float, Cast<double, double>, VResizeNoVec>>,
        0};

    static ResizeFunc lanczos4_tab[] = {
        resizeGeneric_<HResizeLanczos4<uchar, int, short>,
                       VResizeLanczos4<uchar, int, short, FixedPtCast<int, uchar, INTER_RESIZE_COEF_BITS * 2>, VResizeNoVec>>,
        0,
        resizeGeneric_<HResizeLanczos4<ushort, float, float>,
                       VResizeLanczos4<ushort, float, float, Cast<float, ushort>, VResizeLanczos4Vec_32f16u>>,
        resizeGeneric_<HResizeLanczos4<short, float, float>,
                       VResizeLanczos4<short, float, float, Cast<float, short>, VResizeLanczos4Vec_32f16s>>,
        0,
        resizeGeneric_<HResizeLanczos4<float, float, float>,
                       VResizeLanczos4<float, float, float, Cast<float, float>, VResizeLanczos4Vec_32f>>,
        resizeGeneric_<HResizeLanczos4<double, double, float>,
                       VResizeLanczos4<double, double, float, Cast<double, double>, VResizeNoVec>>,
        0};

    static ResizeAreaFastFunc areafast_tab[] = {
        resizeAreaFast_<uchar, int, ResizeAreaFastVec<uchar, ResizeAreaFastVec_SIMD_8u>>,
        0,
        resizeAreaFast_<ushort, float, ResizeAreaFastVec<ushort, ResizeAreaFastVec_SIMD_16u>>,
        resizeAreaFast_<short, float, ResizeAreaFastVec<short, ResizeAreaFastVec_SIMD_16s>>,
        0,
        resizeAreaFast_<float, float, ResizeAreaFastVec_SIMD_32f>,
        resizeAreaFast_<double, double, ResizeAreaFastNoVec<double, double>>,
        0};

    static ResizeAreaFunc area_tab[] = {
        resizeArea_<uchar, float>, 0, resizeArea_<ushort, float>, resizeArea_<short, float>, 0, resizeArea_<float, float>, resizeArea_<double, double>, 0};

    static be_resize_func linear_exact_tab[] = {
        resize_bitExact<uchar, interpolationLinear<uchar>>,
        resize_bitExact<schar, interpolationLinear<schar>>,
        resize_bitExact<ushort, interpolationLinear<ushort>>,
        resize_bitExact<short, interpolationLinear<short>>,
        resize_bitExact<int, interpolationLinear<int>>,
        0,
        0,
        0};

    double scale_x = 1. / inv_scale_x, scale_y = 1. / inv_scale_y;

    int iscale_x      = saturate_cast<int>(scale_x);
    int iscale_y      = saturate_cast<int>(scale_y);

    bool is_area_fast = std::abs(scale_x - iscale_x) < DBL_EPSILON && std::abs(scale_y - iscale_y) < DBL_EPSILON;

    Mat src(Size(src_width, src_height), src_type, const_cast<uchar*>(src_data), src_step);
    Mat dst(dsize, src_type, dst_data, dst_step);

    if (interpolation == INTER_LINEAR_EXACT)
    {
        // in case of inv_scale_x && inv_scale_y is equal to 0.5
        // INTER_AREA (fast) is equal to bit exact INTER_LINEAR
        if (is_area_fast && iscale_x == 2 && iscale_y == 2 && cn != 2)    //Area resize implementation for 2-channel images isn't bit-exact
            interpolation = INTER_AREA;
        else
        {
            be_resize_func func = linear_exact_tab[depth];
            HL_Assert(func != 0);
            func(src_data, src_step, src_width, src_height, dst_data, dst_step, dst_width, dst_height, cn, inv_scale_x, inv_scale_y);
            return;
        }
    }

    if (interpolation == INTER_NEAREST)
    {
        resizeNN(src, dst, inv_scale_x, inv_scale_y);
        return;
    }

    if (interpolation == INTER_NEAREST_EXACT)
    {
        resizeNN_bitexact(src, dst, inv_scale_x, inv_scale_y);
        return;
    }

    int k, sx, sy, dx, dy;


    {
        // in case of scale_x && scale_y is equal to 2
        // INTER_AREA (fast) also is equal to INTER_LINEAR
        if (interpolation == INTER_LINEAR && is_area_fast && iscale_x == 2 && iscale_y == 2)
            interpolation = INTER_AREA;

        // true "area" interpolation is only implemented for the case (scale_x >= 1 && scale_y >= 1).
        // In other cases it is emulated using some variant of bilinear interpolation
        if (interpolation == INTER_AREA && scale_x >= 1 && scale_y >= 1)
        {
            if (is_area_fast)
            {
                int                area    = iscale_x * iscale_y;
                size_t             srcstep = src_step / src.elemSize1();
                AutoBuffer<int>    _ofs(area + dsize.width * cn);
                int*               ofs  = _ofs.data();
                int*               xofs = ofs + area;
                ResizeAreaFastFunc func = areafast_tab[depth];
                HL_Assert(func != 0);

                for (sy = 0, k = 0; sy < iscale_y; sy++)
                    for (sx = 0; sx < iscale_x; sx++)
                        ofs[k++] = (int)(sy * srcstep + sx * cn);

                for (dx = 0; dx < dsize.width; dx++)
                {
                    int j = dx * cn;
                    sx    = iscale_x * j;
                    for (k = 0; k < cn; k++)
                        xofs[j + k] = sx + k;
                }

                func(src, dst, ofs, xofs, iscale_x, iscale_y);
                return;
            }

            ResizeAreaFunc func = area_tab[depth];
            HL_Assert(func != 0 && cn <= 4);

            AutoBuffer<DecimateAlpha> _xytab((src_width + src_height) * 2);
            DecimateAlpha *           xtab = _xytab.data(), *ytab = xtab + src_width * 2;

            int xtab_size = computeResizeAreaTab(src_width, dsize.width, cn, scale_x, xtab);
            int ytab_size = computeResizeAreaTab(src_height, dsize.height, 1, scale_y, ytab);

            AutoBuffer<int> _tabofs(dsize.height + 1);
            int*            tabofs = _tabofs.data();
            for (k = 0, dy = 0; k < ytab_size; k++)
            {
                if (k == 0 || ytab[k].di != ytab[k - 1].di)
                {
                    HL_Assert(ytab[k].di == dy);
                    tabofs[dy++] = k;
                }
            }
            tabofs[dy] = ytab_size;

            func(src, dst, xtab, xtab_size, ytab, ytab_size, tabofs);
            return;
        }
    }

    int        xmin = 0, xmax = dsize.width, width = dsize.width * cn;
    bool       area_mode = interpolation == INTER_AREA;
    bool       fixpt     = depth == HL_8U;
    float      fx, fy;
    ResizeFunc func  = 0;
    int        ksize = 0, ksize2;
    if (interpolation == INTER_CUBIC)
        ksize = 4, func = cubic_tab[depth];
    else if (interpolation == INTER_LANCZOS4)
        ksize = 8, func = lanczos4_tab[depth];
    else if (interpolation == INTER_LINEAR || interpolation == INTER_AREA)
        ksize = 2, func = linear_tab[depth];
    else
        HL_Error(hl::Error::StsBadArg, "Unknown interpolation method");
    ksize2 = ksize / 2;

    HL_Assert(func != 0);

    AutoBuffer<uchar> _buffer((width + dsize.height) * (sizeof(int) + sizeof(float) * ksize));
    int*              xofs            = (int*)_buffer.data();
    int*              yofs            = xofs + width;
    float*            alpha           = (float*)(yofs + dsize.height);
    short*            ialpha          = (short*)alpha;
    float*            beta            = alpha + width * ksize;
    short*            ibeta           = ialpha + width * ksize;
    float             cbuf[MAX_ESIZE] = {0};

    for (dx = 0; dx < dsize.width; dx++)
    {
        if (!area_mode)
        {
            fx  = (float)((dx + 0.5) * scale_x - 0.5);
            sx  = hlFloor(fx);
            fx -= sx;
        }
        else
        {
            sx = hlFloor(dx * scale_x);
            fx = (float)((dx + 1) - (sx + 1) * inv_scale_x);
            fx = fx <= 0 ? 0.f : fx - hlFloor(fx);
        }

        if (sx < ksize2 - 1)
        {
            xmin = dx + 1;
            if (sx < 0 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
                fx = 0, sx = 0;
        }

        if (sx + ksize2 >= src_width)
        {
            xmax = std::min(xmax, dx);
            if (sx >= src_width - 1 && (interpolation != INTER_CUBIC && interpolation != INTER_LANCZOS4))
                fx = 0, sx = src_width - 1;
        }

        for (k = 0, sx *= cn; k < cn; k++)
            xofs[dx * cn + k] = sx + k;

        if (interpolation == INTER_CUBIC)
            interpolateCubic(fx, cbuf);
        else if (interpolation == INTER_LANCZOS4)
            interpolateLanczos4(fx, cbuf);
        else
        {
            cbuf[0] = 1.f - fx;
            cbuf[1] = fx;
        }
        if (fixpt)
        {
            for (k = 0; k < ksize; k++)
                ialpha[dx * cn * ksize + k] = saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
            for (; k < cn * ksize; k++)
                ialpha[dx * cn * ksize + k] = ialpha[dx * cn * ksize + k - ksize];
        }
        else
        {
            for (k = 0; k < ksize; k++)
                alpha[dx * cn * ksize + k] = cbuf[k];
            for (; k < cn * ksize; k++)
                alpha[dx * cn * ksize + k] = alpha[dx * cn * ksize + k - ksize];
        }
    }

    for (dy = 0; dy < dsize.height; dy++)
    {
        if (!area_mode)
        {
            fy  = (float)((dy + 0.5) * scale_y - 0.5);
            sy  = hlFloor(fy);
            fy -= sy;
        }
        else
        {
            sy = hlFloor(dy * scale_y);
            fy = (float)((dy + 1) - (sy + 1) * inv_scale_y);
            fy = fy <= 0 ? 0.f : fy - hlFloor(fy);
        }

        yofs[dy] = sy;
        if (interpolation == INTER_CUBIC)
            interpolateCubic(fy, cbuf);
        else if (interpolation == INTER_LANCZOS4)
            interpolateLanczos4(fy, cbuf);
        else
        {
            cbuf[0] = 1.f - fy;
            cbuf[1] = fy;
        }

        if (fixpt)
        {
            for (k = 0; k < ksize; k++)
                ibeta[dy * ksize + k] = saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
        }
        else
        {
            for (k = 0; k < ksize; k++)
                beta[dy * ksize + k] = cbuf[k];
        }
    }

    func(src, dst, xofs, fixpt ? (void*)ialpha : (void*)alpha, yofs, fixpt ? (void*)ibeta : (void*)beta, xmin, xmax, ksize);
}

}    // namespace hal
}    // namespace hl

void hl::resize(const Mat& _src, Mat& _dst, Size dsize, double inv_scale_x, double inv_scale_y, int interpolation)
{
    Size ssize = _src.size();

    HL_Assert(!ssize.empty());
    if (dsize.empty())
    {
        HL_Assert(inv_scale_x > 0);
        HL_Assert(inv_scale_y > 0);
        dsize = Size(saturate_cast<int>(ssize.width * inv_scale_x),
                     saturate_cast<int>(ssize.height * inv_scale_y));
        HL_Assert(!dsize.empty());
    }
    else
    {
        inv_scale_x = (double)dsize.width / ssize.width;
        inv_scale_y = (double)dsize.height / ssize.height;
        HL_Assert(inv_scale_x > 0);
        HL_Assert(inv_scale_y > 0);
    }

    if (interpolation == INTER_LINEAR_EXACT && (_src.depth() == HL_32F || _src.depth() == HL_64F))
        interpolation = INTER_LINEAR;    // If depth isn't supported fallback to generic resize

    Mat src = _src;
    _dst.create(dsize, src.type());
    Mat dst = _dst;

    if (dsize == ssize)
    {
        // Source and destination are of same size. Use simple copy.
        src.copyTo(dst);
        return;
    }

    hal::resize(src.type(), src.data, src.step, src.cols, src.rows, dst.data, dst.step, dst.cols, dst.rows, inv_scale_x, inv_scale_y, interpolation);
}
