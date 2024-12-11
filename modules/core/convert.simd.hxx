#include "precomp.hxx"

namespace hl
{
namespace cpu_baseline
{

BinaryFunc getConvertFunc(int sdepth, int ddepth);

template <typename _Ts, typename _Td>
inline static void cvt_(const _Ts* src, size_t sstep, _Td* dst, size_t dstep, Size size)
{
    sstep /= sizeof(src[0]);
    dstep /= sizeof(dst[0]);

    for (int i = 0; i < size.height; i++, src += sstep, dst += dstep)
    {
        for (int j = 0; j < size.width; j++)
            dst[j] = saturate_cast<_Td>(src[j]);
    }
}

static void cvtCopy(const uchar* src, size_t sstep, uchar* dst, size_t dstep, Size size, size_t elemsize)
{
    size_t len = size.width * elemsize;
    for (int i = 0; i < size.height; i++, src += sstep, dst += dstep)
    {
        memcpy(dst, src, len);
    }
}

#define DEF_CVT_FUNC(suffix, cvtfunc, _Ts, _Td)                                                                                 \
    static void cvt##suffix(const uchar* src_, size_t sstep, const uchar*, size_t, uchar* dst_, size_t dstep, Size size, void*) \
    {                                                                                                                           \
        const _Ts* src = (const _Ts*)src_;                                                                                      \
        _Td*       dst = (_Td*)dst_;                                                                                            \
        cvtfunc<_Ts, _Td>(src, sstep, dst, dstep, size);                                                                        \
    }

////////////////////// 8u -> ... ////////////////////////

DEF_CVT_FUNC(8u8s, cvt_, uchar, schar)
DEF_CVT_FUNC(8u16u, cvt_, uchar, ushort)
DEF_CVT_FUNC(8u16s, cvt_, uchar, short)
DEF_CVT_FUNC(8u32u, cvt_, uchar, uint)
DEF_CVT_FUNC(8u32s, cvt_, uchar, int)
DEF_CVT_FUNC(8u32f, cvt_, uchar, float)
DEF_CVT_FUNC(8u64f, cvt_, uchar, double)

////////////////////// 8s -> ... ////////////////////////

DEF_CVT_FUNC(8s8u, cvt_, schar, uchar)
DEF_CVT_FUNC(8s16u, cvt_, schar, ushort)
DEF_CVT_FUNC(8s16s, cvt_, schar, short)
DEF_CVT_FUNC(8s32u, cvt_, schar, uint)
DEF_CVT_FUNC(8s32s, cvt_, schar, int)
DEF_CVT_FUNC(8s32f, cvt_, schar, float)
DEF_CVT_FUNC(8s64f, cvt_, schar, double)

////////////////////// 16u -> ... ////////////////////////

DEF_CVT_FUNC(16u8u, cvt_, ushort, uchar)
DEF_CVT_FUNC(16u8s, cvt_, ushort, schar)
DEF_CVT_FUNC(16u16s, cvt_, ushort, short)
DEF_CVT_FUNC(16u32u, cvt_, ushort, uint)
DEF_CVT_FUNC(16u32s, cvt_, ushort, int)
DEF_CVT_FUNC(16u32f, cvt_, ushort, float)
DEF_CVT_FUNC(16u64f, cvt_, ushort, double)

////////////////////// 16s -> ... ////////////////////////

DEF_CVT_FUNC(16s8u, cvt_, short, uchar)
DEF_CVT_FUNC(16s8s, cvt_, short, schar)
DEF_CVT_FUNC(16s16u, cvt_, short, ushort)
DEF_CVT_FUNC(16s32u, cvt_, short, uint)
DEF_CVT_FUNC(16s32s, cvt_, short, int)
DEF_CVT_FUNC(16s32f, cvt_, short, float)
DEF_CVT_FUNC(16s64f, cvt_, short, double)

////////////////////// 32u -> ... ////////////////////////

DEF_CVT_FUNC(32u8u, cvt_, uint, uchar)
DEF_CVT_FUNC(32u8s, cvt_, uint, schar)
DEF_CVT_FUNC(32u16u, cvt_, uint, ushort)
DEF_CVT_FUNC(32u16s, cvt_, uint, short)
DEF_CVT_FUNC(32u32s, cvt_, uint, int)
DEF_CVT_FUNC(32u32f, cvt_, uint, float)
DEF_CVT_FUNC(32u64f, cvt_, uint, double)

////////////////////// 32s -> ... ////////////////////////

DEF_CVT_FUNC(32s8u, cvt_, int, uchar)
DEF_CVT_FUNC(32s8s, cvt_, int, schar)
DEF_CVT_FUNC(32s16u, cvt_, int, ushort)
DEF_CVT_FUNC(32s16s, cvt_, int, short)
DEF_CVT_FUNC(32s32u, cvt_, int, uint)
DEF_CVT_FUNC(32s32f, cvt_, int, float)
DEF_CVT_FUNC(32s64f, cvt_, int, double)

////////////////////// 32f -> ... ////////////////////////

DEF_CVT_FUNC(32f8u, cvt_, float, uchar)
DEF_CVT_FUNC(32f8s, cvt_, float, schar)
DEF_CVT_FUNC(32f16u, cvt_, float, ushort)
DEF_CVT_FUNC(32f16s, cvt_, float, short)
DEF_CVT_FUNC(32f32u, cvt_, float, uint)
DEF_CVT_FUNC(32f32s, cvt_, float, int)
DEF_CVT_FUNC(32f64f, cvt_, float, double)

////////////////////// 64f -> ... ////////////////////////

DEF_CVT_FUNC(64f8u, cvt_, double, uchar)
DEF_CVT_FUNC(64f8s, cvt_, double, schar)
DEF_CVT_FUNC(64f16u, cvt_, double, ushort)
DEF_CVT_FUNC(64f16s, cvt_, double, short)
DEF_CVT_FUNC(64f32u, cvt_, double, uint)
DEF_CVT_FUNC(64f32s, cvt_, double, int)
DEF_CVT_FUNC(64f32f, cvt_, double, float)

///////////// "conversion" w/o conversion ///////////////

static void cvt8(const uchar* src, size_t sstep, const uchar*, size_t, uchar* dst, size_t dstep, Size size, void*)
{
    cvtCopy(src, sstep, dst, dstep, size, 1);
}

static void cvt16(const uchar* src, size_t sstep, const uchar*, size_t, uchar* dst, size_t dstep, Size size, void*)
{
    cvtCopy((const uchar*)src, sstep, (uchar*)dst, dstep, size, 2);
}

static void cvt32(const uchar* src, size_t sstep, const uchar*, size_t, uchar* dst, size_t dstep, Size size, void*)
{
    cvtCopy((const uchar*)src, sstep, (uchar*)dst, dstep, size, 4);
}

static void cvt64(const uchar* src, size_t sstep, const uchar*, size_t, uchar* dst, size_t dstep, Size size, void*)
{
    cvtCopy((const uchar*)src, sstep, (uchar*)dst, dstep, size, 8);
}

BinaryFunc getConvertFunc(int sdepth, int ddepth)
{
    static BinaryFunc cvtTab[][8] = {
        {(cvt8), (cvt8u8s), (cvt8u16u), (cvt8u16s), (cvt8u32u), (cvt8u32s), (cvt8u32f), (cvt8u64f)},
        {(cvt8s8u), (cvt8), (cvt8s16u), (cvt8s16s), (cvt8s32u), (cvt8s32s), (cvt8s32f), (cvt8s64f)},
        {(cvt16u8u), (cvt16u8s), (cvt16), (cvt16u16s), (cvt16u32u), (cvt16u32s), (cvt16u32f), (cvt16u64f)},
        {(cvt16s8u), (cvt16s8s), (cvt16s16u), (cvt16), (cvt16s32u), (cvt16s32s), (cvt16s32f), (cvt16s64f)},
        {(cvt32u8u), (cvt32u8s), (cvt32u16u), (cvt32u16s), (cvt32), (cvt32u32s), (cvt32u32f), (cvt32u64f)},
        {(cvt32s8u), (cvt32s8s), (cvt32s16u), (cvt32s16s), (cvt32s32u), (cvt32), (cvt32s32f), (cvt32s64f)},
        {(cvt32f8u), (cvt32f8s), (cvt32f16u), (cvt32f16s), (cvt32f32u), (cvt32f32s), (cvt32), (cvt32f64f)},
        {(cvt64f8u), (cvt64f8s), (cvt64f16u), (cvt64f16s), (cvt64f32u), (cvt64f32s), (cvt64f32f), (cvt64)}};
    return cvtTab[HL_MAT_DEPTH(sdepth)][HL_MAT_DEPTH(ddepth)];
}

}    // namespace cpu_baseline
}    // namespace hl