#pragma once

#include <limits>
#include "openHL/core/hal/interface.h"
#include <cstdint>


//////////////// static assert /////////////////

#define HL_StaticAssert(condition, reason) static_assert((condition), reason " " #condition)

#define HL_UNUSED(name)                    (void)name

#ifdef __GNUC__
    #define HL_DECL_ALIGNED(x) __attribute__((aligned(x)))
#elif defined _MSC_VER
    #define HL_DECL_ALIGNED(x) __declspec(align(x))
#else
    #define HL_DECL_ALIGNED(x)
#endif

#ifndef HL_ALWAYS_INLINE
    #if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
        #define HL_ALWAYS_INLINE inline __attribute__((always_inline))
    #elif defined(_MSC_VER)
        #define HL_ALWAYS_INLINE __forceinline
    #else
        #define HL_ALWAYS_INLINE inline
    #endif
#endif

#ifdef __GNUC__
    #define HL_DECL_ALIGNED(x) __attribute__((aligned(x)))
#elif defined _MSC_VER
    #define HL_DECL_ALIGNED(x) __declspec(align(x))
#else
    #define HL_DECL_ALIGNED(x)
#endif

/* fundamental constants */
#define HL_PI   3.1415926535897932384626433832795
#define HL_2PI  6.283185307179586476925286766559
#define HL_LOG2 0.69314718055994530941723212145818

typedef union Hl32suf
{
    int      i;
    unsigned u;
    float    f;
} Hl32suf;

typedef union Hl64suf
{
    int64  i;
    uint64 u;
    double f;
} Hl64suf;

/****************************************************************************************\
*                                  Matrix type (Mat)                                     *
\****************************************************************************************/

#define HL_MAT_CN_MASK         ((HL_CN_MAX - 1) << HL_CN_SHIFT)
#define HL_MAT_CN(flags)       ((((flags) & HL_MAT_CN_MASK) >> HL_CN_SHIFT) + 1)
#define HL_MAT_TYPE_MASK       (HL_DEPTH_MAX * HL_CN_MAX - 1)
#define HL_MAT_TYPE(flags)     ((flags) & HL_MAT_TYPE_MASK)
#define HL_MAT_CONT_FLAG_SHIFT 14
#define HL_MAT_CONT_FLAG       (1 << HL_MAT_CONT_FLAG_SHIFT)
#define HL_IS_MAT_CONT(flags)  ((flags) & HL_MAT_CONT_FLAG)
#define HL_IS_CONT_MAT         HL_IS_MAT_CONT
#define HL_SUBMAT_FLAG_SHIFT   15
#define HL_SUBMAT_FLAG         (1 << HL_SUBMAT_FLAG_SHIFT)
#define HL_IS_SUBMAT(flags)    ((flags) & HL_MAT_SUBMAT_FLAG)

/** Size of each channel item,
   0x84442211 = 1000 0100 0100 0100 0010 0010 0001 0001 ~ array of sizeof(arr_type_elem) */
#define HL_ELEM_SIZE1(type)    ((0x84442211 >> HL_MAT_DEPTH(type) * 4) & 15)


#define HL_ELEM_SIZE(type)     (HL_MAT_CN(type) * HL_ELEM_SIZE1(type))

#ifndef MIN
    #define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
    #define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif

///////////////////////////////////////// Enum operators ///////////////////////////////////////

#define __HL_ENUM_FLAGS_LOGICAL_NOT(EnumType)                        \
    static inline bool operator!(const EnumType& val)                \
    {                                                                \
        typedef std::underlying_type<EnumType>::type UnderlyingType; \
        return !static_cast<UnderlyingType>(val);                    \
    }

#define __HL_ENUM_FLAGS_LOGICAL_NOT_EQ(Arg1Type, Arg2Type)              \
    static inline bool operator!=(const Arg1Type& a, const Arg2Type& b) \
    {                                                                   \
        return static_cast<int>(a) != static_cast<int>(b);              \
    }

#define __HL_ENUM_FLAGS_LOGICAL_EQ(Arg1Type, Arg2Type)                  \
    static inline bool operator==(const Arg1Type& a, const Arg2Type& b) \
    {                                                                   \
        return static_cast<int>(a) == static_cast<int>(b);              \
    }

#define __HL_ENUM_FLAGS_BITWISE_NOT(EnumType)                            \
    static inline EnumType operator~(const EnumType& val)                \
    {                                                                    \
        typedef std::underlying_type<EnumType>::type UnderlyingType;     \
        return static_cast<EnumType>(~static_cast<UnderlyingType>(val)); \
    }

#define __HL_ENUM_FLAGS_BITWISE_OR(EnumType, Arg1Type, Arg2Type)                                       \
    static inline EnumType operator|(const Arg1Type& a, const Arg2Type& b)                             \
    {                                                                                                  \
        typedef std::underlying_type<EnumType>::type UnderlyingType;                                   \
        return static_cast<EnumType>(static_cast<UnderlyingType>(a) | static_cast<UnderlyingType>(b)); \
    }

#define __HL_ENUM_FLAGS_BITWISE_AND(EnumType, Arg1Type, Arg2Type)                                      \
    static inline EnumType operator&(const Arg1Type& a, const Arg2Type& b)                             \
    {                                                                                                  \
        typedef std::underlying_type<EnumType>::type UnderlyingType;                                   \
        return static_cast<EnumType>(static_cast<UnderlyingType>(a) & static_cast<UnderlyingType>(b)); \
    }

#define __HL_ENUM_FLAGS_BITWISE_XOR(EnumType, Arg1Type, Arg2Type)                                      \
    static inline EnumType operator^(const Arg1Type& a, const Arg2Type& b)                             \
    {                                                                                                  \
        typedef std::underlying_type<EnumType>::type UnderlyingType;                                   \
        return static_cast<EnumType>(static_cast<UnderlyingType>(a) ^ static_cast<UnderlyingType>(b)); \
    }

#define __HL_ENUM_FLAGS_BITWISE_OR_EQ(EnumType, Arg1Type)                               \
    static inline EnumType& operator|=(EnumType& _this, const Arg1Type& val)            \
    {                                                                                   \
        _this = static_cast<EnumType>(static_cast<int>(_this) | static_cast<int>(val)); \
        return _this;                                                                   \
    }

#define __HL_ENUM_FLAGS_BITWISE_AND_EQ(EnumType, Arg1Type)                              \
    static inline EnumType& operator&=(EnumType& _this, const Arg1Type& val)            \
    {                                                                                   \
        _this = static_cast<EnumType>(static_cast<int>(_this) & static_cast<int>(val)); \
        return _this;                                                                   \
    }

#define __HL_ENUM_FLAGS_BITWISE_XOR_EQ(EnumType, Arg1Type)                              \
    static inline EnumType& operator^=(EnumType& _this, const Arg1Type& val)            \
    {                                                                                   \
        _this = static_cast<EnumType>(static_cast<int>(_this) ^ static_cast<int>(val)); \
        return _this;                                                                   \
    }

#define HL_ENUM_FLAGS(EnumType)                               \
    __HL_ENUM_FLAGS_LOGICAL_NOT(EnumType)                     \
    __HL_ENUM_FLAGS_LOGICAL_EQ(EnumType, int)                 \
    __HL_ENUM_FLAGS_LOGICAL_NOT_EQ(EnumType, int)             \
                                                              \
    __HL_ENUM_FLAGS_BITWISE_NOT(EnumType)                     \
    __HL_ENUM_FLAGS_BITWISE_OR(EnumType, EnumType, EnumType)  \
    __HL_ENUM_FLAGS_BITWISE_AND(EnumType, EnumType, EnumType) \
    __HL_ENUM_FLAGS_BITWISE_XOR(EnumType, EnumType, EnumType) \
                                                              \
    __HL_ENUM_FLAGS_BITWISE_OR_EQ(EnumType, EnumType)         \
    __HL_ENUM_FLAGS_BITWISE_AND_EQ(EnumType, EnumType)        \
    __HL_ENUM_FLAGS_BITWISE_XOR_EQ(EnumType, EnumType)

/****************************************************************************************\
*          exchange-add operation for atomic operations on reference counters            *
\****************************************************************************************/

#if defined __GNUC__ || defined __clang__
    #if defined __clang__ && __clang_major__ >= 3 && !defined __ANDROID__ && !defined __EMSCRIPTEN__ && !defined(__CUDACC__) && !defined __INTEL_COMPILER
        #ifdef __ATOMIC_ACQ_REL
            #define HL_XADD(addr, delta) __c11_atomic_fetch_add((_Atomic(int)*)(addr), delta, __ATOMIC_ACQ_REL)
        #else
            #define HL_XADD(addr, delta) __atomic_fetch_add((_Atomic(int)*)(addr), delta, 4)
        #endif
    #else
        #if defined __ATOMIC_ACQ_REL && !defined __clang__
        // version for gcc >= 4.7
            #define HL_XADD(addr, delta) (int)__atomic_fetch_add((unsigned*)(addr), (unsigned)(delta), __ATOMIC_ACQ_REL)
        #else
            #define HL_XADD(addr, delta) (int)__sync_fetch_and_add((unsigned*)(addr), (unsigned)(delta))
        #endif
    #endif
#endif

#include "openHL/core/fast_math.hxx"