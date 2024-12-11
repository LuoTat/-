#include "precomp.hxx"

namespace hl
{
namespace cpu_baseline
{

BinaryFunc getConvertScaleFunc(int sdepth, int ddepth);

template <typename _Ts, typename _Td>
inline void cvt_32f(const _Ts* src, size_t sstep, _Td* dst, size_t dstep, Size size, float a, float b)
{
    sstep /= sizeof(src[0]);
    dstep /= sizeof(dst[0]);

    for (int i = 0; i < size.height; i++, src += sstep, dst += dstep)
    {
        for (int j = 0; j < size.width; j++)
            dst[j] = saturate_cast<_Td>(src[j] * a + b);
    }
}

template <typename _Ts, typename _Td>
inline void cvt_64f(const _Ts* src, size_t sstep, _Td* dst, size_t dstep, Size size, double a, double b)
{
    sstep /= sizeof(src[0]);
    dstep /= sizeof(dst[0]);

    for (int i = 0; i < size.height; i++, src += sstep, dst += dstep)
    {
        for (int j = 0; j < size.width; j++)
            dst[j] = saturate_cast<_Td>(src[j] * a + b);
    }
}

#define DEF_CVT_SCALE_FUNC(suffix, cvt, stype, dtype, wtype)                                                                                \
    static void cvtScale##suffix(const uchar* src_, size_t sstep, const uchar*, size_t, uchar* dst_, size_t dstep, Size size, void* scale_) \
    {                                                                                                                                       \
        const stype* src   = (const stype*)src_;                                                                                            \
        dtype*       dst   = (dtype*)dst_;                                                                                                  \
        double*      scale = (double*)scale_;                                                                                               \
        cvt(src, sstep, dst, dstep, size, (wtype)scale[0], (wtype)scale[1]);                                                                \
    }
////////////////////// 8u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(8u, cvt_32f, uchar, uchar, float)
DEF_CVT_SCALE_FUNC(8u8s, cvt_32f, uchar, schar, float)
DEF_CVT_SCALE_FUNC(8u16u, cvt_32f, uchar, ushort, float)
DEF_CVT_SCALE_FUNC(8u16s, cvt_32f, uchar, short, float)
DEF_CVT_SCALE_FUNC(8u32u, cvt_32f, uchar, uint, float)
DEF_CVT_SCALE_FUNC(8u32s, cvt_32f, uchar, int, float)
DEF_CVT_SCALE_FUNC(8u32f, cvt_32f, uchar, float, float)
DEF_CVT_SCALE_FUNC(8u64f, cvt_64f, uchar, double, double)

////////////////////// 8s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(8s8u, cvt_32f, schar, uchar, float)
DEF_CVT_SCALE_FUNC(8s, cvt_32f, schar, schar, float)
DEF_CVT_SCALE_FUNC(8s16u, cvt_32f, schar, ushort, float)
DEF_CVT_SCALE_FUNC(8s16s, cvt_32f, schar, short, float)
DEF_CVT_SCALE_FUNC(8s32u, cvt_32f, schar, uint, float)
DEF_CVT_SCALE_FUNC(8s32s, cvt_32f, schar, int, float)
DEF_CVT_SCALE_FUNC(8s32f, cvt_32f, schar, float, float)
DEF_CVT_SCALE_FUNC(8s64f, cvt_64f, schar, double, double)

////////////////////// 16u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(16u8u, cvt_32f, ushort, uchar, float)
DEF_CVT_SCALE_FUNC(16u8s, cvt_32f, ushort, schar, float)
DEF_CVT_SCALE_FUNC(16u, cvt_32f, ushort, ushort, float)
DEF_CVT_SCALE_FUNC(16u16s, cvt_32f, ushort, short, float)
DEF_CVT_SCALE_FUNC(16u32u, cvt_32f, ushort, uint, float)
DEF_CVT_SCALE_FUNC(16u32s, cvt_32f, ushort, int, float)
DEF_CVT_SCALE_FUNC(16u32f, cvt_32f, ushort, float, float)
DEF_CVT_SCALE_FUNC(16u64f, cvt_64f, ushort, double, double)

////////////////////// 16s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(16s8u, cvt_32f, short, uchar, float)
DEF_CVT_SCALE_FUNC(16s8s, cvt_32f, short, schar, float)
DEF_CVT_SCALE_FUNC(16s16u, cvt_32f, short, ushort, float)
DEF_CVT_SCALE_FUNC(16s, cvt_32f, short, short, float)
DEF_CVT_SCALE_FUNC(16s32u, cvt_32f, short, uint, float)
DEF_CVT_SCALE_FUNC(16s32s, cvt_32f, short, int, float)
DEF_CVT_SCALE_FUNC(16s32f, cvt_32f, short, float, float)
DEF_CVT_SCALE_FUNC(16s64f, cvt_64f, short, double, double)

////////////////////// 32u -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32u8u, cvt_32f, uint, uchar, float)
DEF_CVT_SCALE_FUNC(32u8s, cvt_32f, uint, schar, float)
DEF_CVT_SCALE_FUNC(32u16u, cvt_32f, uint, ushort, float)
DEF_CVT_SCALE_FUNC(32u16s, cvt_32f, uint, short, float)
DEF_CVT_SCALE_FUNC(32u, cvt_32f, uint, uint, float)
DEF_CVT_SCALE_FUNC(32u32s, cvt_32f, uint, int, float)
DEF_CVT_SCALE_FUNC(32u32f, cvt_32f, uint, float, float)
DEF_CVT_SCALE_FUNC(32u64f, cvt_64f, uint, double, double)

////////////////////// 32s -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32s8u, cvt_32f, int, uchar, float)
DEF_CVT_SCALE_FUNC(32s8s, cvt_32f, int, schar, float)
DEF_CVT_SCALE_FUNC(32s16u, cvt_32f, int, ushort, float)
DEF_CVT_SCALE_FUNC(32s16s, cvt_32f, int, short, float)
DEF_CVT_SCALE_FUNC(32s32u, cvt_32f, int, uint, float)
DEF_CVT_SCALE_FUNC(32s, cvt_32f, int, int, float)
DEF_CVT_SCALE_FUNC(32s32f, cvt_32f, int, float, float)
DEF_CVT_SCALE_FUNC(32s64f, cvt_64f, int, double, double)

////////////////////// 32f -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(32f8u, cvt_32f, float, uchar, float)
DEF_CVT_SCALE_FUNC(32f8s, cvt_32f, float, schar, float)
DEF_CVT_SCALE_FUNC(32f16u, cvt_32f, float, ushort, float)
DEF_CVT_SCALE_FUNC(32f16s, cvt_32f, float, short, float)
DEF_CVT_SCALE_FUNC(32f32u, cvt_32f, float, uint, float)
DEF_CVT_SCALE_FUNC(32f32s, cvt_32f, float, int, float)
DEF_CVT_SCALE_FUNC(32f, cvt_32f, float, float, float)
DEF_CVT_SCALE_FUNC(32f64f, cvt_64f, float, double, double)

////////////////////// 64f -> ... ////////////////////////

DEF_CVT_SCALE_FUNC(64f8u, cvt_32f, double, uchar, float)
DEF_CVT_SCALE_FUNC(64f8s, cvt_32f, double, schar, float)
DEF_CVT_SCALE_FUNC(64f16u, cvt_32f, double, ushort, float)
DEF_CVT_SCALE_FUNC(64f16s, cvt_32f, double, short, float)
DEF_CVT_SCALE_FUNC(64f32u, cvt_64f, double, uint, double)
DEF_CVT_SCALE_FUNC(64f32s, cvt_64f, double, int, double)
DEF_CVT_SCALE_FUNC(64f32f, cvt_64f, double, float, double)
DEF_CVT_SCALE_FUNC(64f, cvt_64f, double, double, double)

BinaryFunc getConvertScaleFunc(int sdepth, int ddepth)
{
    static BinaryFunc cvtScaleTab[][8] = {
        {(cvtScale8u), (cvtScale8u8s), (cvtScale8u16u), (cvtScale8u16s), (cvtScale8u32u), (cvtScale8u32s), (cvtScale8u32f), (cvtScale8u64f)},
        {(cvtScale8s8u), (cvtScale8s), (cvtScale8s16u), (cvtScale8s16s), (cvtScale8s32u), (cvtScale8s32s), (cvtScale8s32f), (cvtScale8s64f)},
        {(cvtScale16u8u), (cvtScale16u8s), (cvtScale16u), (cvtScale16u16s), (cvtScale16u32u), (cvtScale16u32s), (cvtScale16u32f), (cvtScale16u64f)},
        {(cvtScale16s8u), (cvtScale16s8s), (cvtScale16s16u), (cvtScale16s), (cvtScale16s32u), (cvtScale16s32s), (cvtScale16s32f), (cvtScale16s64f)},
        {(cvtScale32u8u), (cvtScale32u8s), (cvtScale32u16u), (cvtScale32u16s), (cvtScale32u), (cvtScale32u32s), (cvtScale32u32f), (cvtScale32u64f)},
        {(cvtScale32s8u), (cvtScale32s8s), (cvtScale32s16u), (cvtScale32s16s), (cvtScale32s32u), (cvtScale32s), (cvtScale32s32f), (cvtScale32s64f)},
        {(cvtScale32f8u), (cvtScale32f8s), (cvtScale32f16u), (cvtScale32f16s), (cvtScale32f32u), (cvtScale32f32s), (cvtScale32f), (cvtScale32f64f)},
        {(cvtScale64f8u), (cvtScale64f8s), (cvtScale64f16u), (cvtScale64f16s), (cvtScale64f32u), (cvtScale64f32s), (cvtScale64f32f), (cvtScale64f)}};
    return cvtScaleTab[HL_MAT_DEPTH(sdepth)][HL_MAT_DEPTH(ddepth)];
}

}    // namespace cpu_baseline
}    // namespace hl