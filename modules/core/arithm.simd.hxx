#include "openHL/core/hal/intrin.hxx"

#undef ARITHM_DECLARATIONS_ONLY
#ifdef HL_CPU_OPTIMIZATION_DECLARATIONS_ONLY
    #define ARITHM_DECLARATIONS_ONLY
#endif

#undef ARITHM_DEFINITIONS_ONLY
#if !defined(HL_CPU_OPTIMIZATION_DECLARATIONS_ONLY) && !defined(ARITHM_DISPATCHING_ONLY)
    #define ARITHM_DEFINITIONS_ONLY
#endif

#ifdef ARITHM_DECLARATIONS_ONLY
    #undef DEFINE_SIMD
    #define DEFINE_SIMD(fun_name, c_type, ...) \
        DECLARE_SIMD_FUN(fun_name, c_type)
#endif    // ARITHM_DECLARATIONS_ONLY

#ifdef ARITHM_DEFINITIONS_ONLY
    #undef DEFINE_SIMD
    #define DEFINE_SIMD(fun_name, c_type, ...) \
        DECLARE_SIMD_FUN(fun_name, c_type)     \
        DEFINE_SIMD_FUN(fun_name, c_type, __VA_ARGS__)
#endif    // ARITHM_DEFINITIONS_ONLY

#ifdef ARITHM_DISPATCHING_ONLY
    #undef DEFINE_SIMD
    #define DEFINE_SIMD(fun_name, c_type, ...) \
        DISPATCH_SIMD_FUN(fun_name, c_type, __VA_ARGS__)
#endif    // ARITHM_DISPATCHING_ONLY

#ifndef SIMD_GUARD

    #define DEFINE_SIMD_U8(fun, ...) \
        DEFINE_SIMD(fun##8u, uchar, __VA_ARGS__)

    #define DEFINE_SIMD_S8(fun, ...) \
        DEFINE_SIMD(fun##8s, schar, __VA_ARGS__)

    #define DEFINE_SIMD_U16(fun, ...) \
        DEFINE_SIMD(fun##16u, ushort, __VA_ARGS__)

    #define DEFINE_SIMD_S16(fun, ...) \
        DEFINE_SIMD(fun##16s, short, __VA_ARGS__)

    #define DEFINE_SIMD_U32(fun, ...) \
        DEFINE_SIMD(fun##32u, uint, __VA_ARGS__)

    #define DEFINE_SIMD_S32(fun, ...) \
        DEFINE_SIMD(fun##32s, int, __VA_ARGS__)

    #define DEFINE_SIMD_F32(fun, ...) \
        DEFINE_SIMD(fun##32f, float, __VA_ARGS__)

    #define DEFINE_SIMD_F64(fun, ...) \
        DEFINE_SIMD(fun##64f, double, __VA_ARGS__)

    #define DEFINE_SIMD_SAT(fun, ...)     \
        DEFINE_SIMD_U8(fun, __VA_ARGS__)  \
        DEFINE_SIMD_S8(fun, __VA_ARGS__)  \
        DEFINE_SIMD_U16(fun, __VA_ARGS__) \
        DEFINE_SIMD_S16(fun, __VA_ARGS__)

    #define DEFINE_SIMD_NSAT(fun, ...)    \
        DEFINE_SIMD_U32(fun, __VA_ARGS__) \
        DEFINE_SIMD_S32(fun, __VA_ARGS__) \
        DEFINE_SIMD_F32(fun, __VA_ARGS__) \
        DEFINE_SIMD_F64(fun, __VA_ARGS__)

    #define DEFINE_SIMD_ALL(fun, ...)     \
        DEFINE_SIMD_SAT(fun, __VA_ARGS__) \
        DEFINE_SIMD_NSAT(fun, __VA_ARGS__)

#endif    // SIMD_GUARD

namespace hl
{
namespace hal
{
#ifndef ARITHM_DISPATCHING_ONLY
namespace cpu_baseline
{
#endif

//=======================================
// Utility
//=======================================

/** add **/
template <typename T>
inline static T c_add(T a, T b)
{
    return saturate_cast<T>(a + b);
}

template <>
inline uchar c_add<uchar>(uchar a, uchar b)
{
    return HL_FAST_CAST_8U(a + b);
}

// scale
template <typename T1, typename T2>
inline static T1 c_add(T1 a, T1 b, T2 scalar)
{
    return saturate_cast<T1>((T2)a * scalar + b);
}

template <>
inline uchar c_add<uchar, float>(uchar a, uchar b, float scalar)
{
    return saturate_cast<uchar>(HL_8TO32F(a) * scalar + b);
}

// weight
template <typename T1, typename T2>
inline static T1 c_add(T1 a, T1 b, T2 alpha, T2 beta, T2 gamma)
{
    return saturate_cast<T1>(a * alpha + b * beta + gamma);
}

template <>
inline uchar c_add<uchar, float>(uchar a, uchar b, float alpha, float beta, float gamma)
{
    return saturate_cast<uchar>(HL_8TO32F(a) * alpha + HL_8TO32F(b) * beta + gamma);
}

/** sub **/
template <typename T>
inline static T c_sub(T a, T b)
{
    return saturate_cast<T>(a - b);
}

template <>
inline uchar c_sub<uchar>(uchar a, uchar b)
{
    return HL_FAST_CAST_8U(a - b);
}

/** max **/
template <typename T>
inline static T c_max(T a, T b)
{
    return std::max(a, b);
}

template <>
inline uchar c_max<uchar>(uchar a, uchar b)
{
    return HL_MAX_8U(a, b);
}

/** min **/
template <typename T>
inline static T c_min(T a, T b)
{
    return std::min(a, b);
}

template <>
inline uchar c_min<uchar>(uchar a, uchar b)
{
    return HL_MIN_8U(a, b);
}

/** absdiff **/
template <typename T>
inline static T c_absdiff(T a, T b)
{
    return a > b ? a - b : b - a;
}

template <>
inline schar c_absdiff(schar a, schar b)
{
    return saturate_cast<schar>(std::abs(a - b));
}

template <>
inline short c_absdiff(short a, short b)
{
    return saturate_cast<short>(std::abs(a - b));
}

// specializations to prevent "-0" results
template <>
inline float c_absdiff<float>(float a, float b)
{
    return std::abs(a - b);
}

template <>
inline double c_absdiff<double>(double a, double b)
{
    return std::abs(a - b);
}

/** multiply **/
template <typename T>
inline static T c_mul(T a, T b)
{
    return saturate_cast<T>(a * b);
}

template <>
inline uchar c_mul<uchar>(uchar a, uchar b)
{
    return HL_FAST_CAST_8U(a * b);
}

// scale
template <typename T1, typename T2>
inline static T1 c_mul(T1 a, T1 b, T2 scalar)
{
    return saturate_cast<T1>(scalar * (T2)a * b);
}

template <>
inline uchar c_mul<uchar, float>(uchar a, uchar b, float scalar)
{
    return saturate_cast<uchar>(scalar * HL_8TO32F(a) * HL_8TO32F(b));
}

/** divide & reciprocal **/
template <typename T1, typename T2>
inline static T2 c_div(T1 a, T2 b)
{
    return saturate_cast<T2>(a / b);
}

// recip
template <>
inline uchar c_div<float, uchar>(float a, uchar b)
{
    return saturate_cast<uchar>(a / HL_8TO32F(b));
}

// scale
template <typename T1, typename T2>
inline static T1 c_div(T1 a, T1 b, T2 scalar)
{
    return saturate_cast<T1>(scalar * (T2)a / b);
}

template <>
inline uchar c_div<uchar, float>(uchar a, uchar b, float scalar)
{
    return saturate_cast<uchar>(scalar * HL_8TO32F(a) / HL_8TO32F(b));
}

///////////////////////////// Operations //////////////////////////////////

// Add
template <typename T1>
struct op_add
{
    inline static T1 r(T1 a, T1 b)
    {
        return c_add(a, b);
    }
};

// Subtract
template <typename T1>
struct op_sub
{
    inline static T1 r(T1 a, T1 b)
    {
        return c_sub(a, b);
    }
};

// Max & Min
template <typename T1>
struct op_max
{
    inline static T1 r(T1 a, T1 b)
    {
        return c_max(a, b);
    }
};

template <typename T1>
struct op_min
{
    inline static T1 r(T1 a, T1 b)
    {
        return c_min(a, b);
    }
};

// Absolute difference
template <typename T1>
struct op_absdiff
{
    inline static T1 r(T1 a, T1 b)
    {
        return c_absdiff(a, b);
    }
};

// Logical
template <typename T1>
struct op_or
{
    inline static T1 r(T1 a, T1 b)
    {
        return a | b;
    }
};

template <typename T1>
struct op_xor
{
    inline static T1 r(T1 a, T1 b)
    {
        return a ^ b;
    }
};

template <typename T1>
struct op_and
{
    inline static T1 r(T1 a, T1 b)
    {
        return a & b;
    }
};

template <typename T1>
struct op_not
{
    inline static T1 r(T1 a, T1)
    {
        return ~a;
    }
};

//////////////////////////// Loops /////////////////////////////////

template <typename T1, typename T2>
inline static bool is_aligned(const T1* src1, const T1* src2, const T2* dst)
{
    return (((size_t)src1 | (size_t)src2 | (size_t)dst) & (HL_SIMD_WIDTH - 1)) == 0;
}

template <template <typename T1> class OP, typename T1>
static void bin_loop_nosimd(const T1* src1, size_t step1, const T1* src2, size_t step2, T1* dst, size_t step, int width, int height)
{
    typedef OP<T1> op;

    step1 /= sizeof(T1);
    step2 /= sizeof(T1);
    step  /= sizeof(T1);

    for (; height--; src1 += step1, src2 += step2, dst += step)
    {
        int x = 0;

        for (; x <= width - 4; x += 4)
        {
            T1 t0      = op::r(src1[x], src2[x]);
            T1 t1      = op::r(src1[x + 1], src2[x + 1]);
            dst[x]     = t0;
            dst[x + 1] = t1;

            t0         = op::r(src1[x + 2], src2[x + 2]);
            t1         = op::r(src1[x + 3], src2[x + 3]);
            dst[x + 2] = t0;
            dst[x + 3] = t1;
        }

        for (; x < width; x++)
            dst[x] = op::r(src1[x], src2[x]);
    }
}

////////////////////////////////////////////////////////////////////////////////////

#ifndef SIMD_GUARD
    #define BIN_ARGS(_T1) const _T1 *src1, size_t step1, const _T1 *src2, size_t step2, \
                          _T1 *dst, size_t step, int width, int height

    #define BIN_ARGS_PASS src1, step1, src2, step2, dst, step, width, height
#endif    // SIMD_GUARD

#undef DECLARE_SIMD_FUN
#define DECLARE_SIMD_FUN(fun, _T1) void fun(BIN_ARGS(_T1));

#undef DISPATCH_SIMD_FUN
#define DISPATCH_SIMD_FUN(fun, _T1, _OP)  \
    void fun(BIN_ARGS(_T1), void*)        \
    {                                     \
        cpu_baseline::fun(BIN_ARGS_PASS); \
    }

#undef DEFINE_SIMD_FUN
#define DEFINE_SIMD_FUN(fun, _T1, _OP)            \
    void fun(BIN_ARGS(_T1))                       \
    {                                             \
        bin_loop_nosimd<_OP, _T1>(BIN_ARGS_PASS); \
    }

DEFINE_SIMD_ALL(add, op_add)
DEFINE_SIMD_ALL(sub, op_sub)

DEFINE_SIMD_ALL(min, op_min)
DEFINE_SIMD_ALL(max, op_max)

DEFINE_SIMD_ALL(absdiff, op_absdiff)

DEFINE_SIMD_U8(or, op_or)
DEFINE_SIMD_U8(xor, op_xor)
DEFINE_SIMD_U8(and, op_and)
DEFINE_SIMD_U8(not, op_not)

#ifndef ARITHM_DISPATCHING_ONLY
}    // namespace cpu_baseline
#endif

#ifndef SIMD_GUARD
    #define SIMD_GUARD
#endif

}    // namespace hal
}    // namespace hl