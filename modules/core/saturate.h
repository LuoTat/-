#pragma once

#include "openHL/core/hldef.h"
#include <limits.h>
#include "openHL/core/fast_math.h"

////////////////////// 8u -> ... ////////////////////////

#define saturate_cast8u8u(x)   (x)
#define saturate_cast8u8s(x)   ((char)min((int)(x), SCHAR_MAX))
#define saturate_cast8u16u(x)  ((unsigned short)(x))
#define saturate_cast8u16s(x)  ((short)(x))
#define saturate_cast8u32u(x)  ((unsigned int)(x))
#define saturate_cast8u32s(x)  ((int)(x))
#define saturate_cast8u64u(x)  ((unsigned long)(x))
#define saturate_cast8u64s(x)  ((long)(x))
#define saturate_cast8u32f(x)  ((float)(x))
#define saturate_cast8u64f(x)  ((double)(x))

////////////////////// 8s -> ... ////////////////////////

#define saturate_cast8s8u(x)   ((unsigned char)max((int)(x), 0))
#define saturate_cast8s8s(x)   (x)
#define saturate_cast8s16u(x)  ((unsigned short)max((int)(x), 0))
#define saturate_cast8s16s(x)  ((short)(x))
#define saturate_cast8s32u(x)  ((unsigned int)max((x), (char)0))
#define saturate_cast8s32s(x)  ((int)(x))
#define saturate_cast8s64u(x)  ((unsigned long)max((x), (char)0))
#define saturate_cast8s64s(x)  ((long)(x))
#define saturate_cast8s32f(x)  ((float)(x))
#define saturate_cast8s64f(x)  ((double)(x))

////////////////////// 16u -> ... ////////////////////////

#define saturate_cast16u8u(x)  ((unsigned char)min((unsigned)(x), (unsigned)UCHAR_MAX))
#define saturate_cast16u8s(x)  ((char)min((unsigned)(x), (unsigned)SCHAR_MAX))
#define saturate_cast16u16u(x) (x)
#define saturate_cast16u16s(x) ((short)min((int)(x), SHRT_MAX))
#define saturate_cast16u32u(x) ((unsigned int)(x))
#define saturate_cast16u32s(x) ((int)(x))
#define saturate_cast16u64u(x) ((unsigned long)(x))
#define saturate_cast16u64s(x) ((long)(x))
#define saturate_cast16u32f(x) ((float)(x))
#define saturate_cast16u64f(x) ((double)(x))

////////////////////// 16s -> ... ////////////////////////

#define saturate_cast16s8u(x)  (saturate_cast32s8u((int)(x)))
#define saturate_cast16s8s(x)  (saturate_cast32s8s((int)(x)))
#define saturate_cast16s16u(x) ((unsigned short)max((int)(x), 0))
#define saturate_cast16s16s(x) (x)
#define saturate_cast16s32u(x) ((unsigned int)max((x), (short)0))
#define saturate_cast16s32s(x) ((int)(x))
#define saturate_cast16s64u(x) ((unsigned long)max((x), (short)0))
#define saturate_cast16s64s(x) ((long)(x))
#define saturate_cast16s32f(x) ((float)(x))
#define saturate_cast16s64f(x) ((double)(x))

////////////////////// 32u -> ... ////////////////////////

#define saturate_cast32u8u(x)  ((unsigned char)min((x), (unsigned int)UCHAR_MAX))
#define saturate_cast32u8s(x)  ((char)min((x), (unsigned int)SCHAR_MAX))
#define saturate_cast32u16u(x) ((unsigned short)min((x), (unsigned int)USHRT_MAX))
#define saturate_cast32u16s(x) ((short)min((x), (unsigned int)SHRT_MAX))
#define saturate_cast32u32u(x) (x)
#define saturate_cast32u32s(x) ((int)min((x), (unsigned int)INT_MAX))
#define saturate_cast32u64u(x) ((unsigned long)(x))
#define saturate_cast32u64s(x) ((long)(x))
#define saturate_cast32u32f(x) ((float)(x))
#define saturate_cast32u64f(x) ((double)(x))

////////////////////// 32s -> ... ////////////////////////

#define saturate_cast32s8u(x)  ((unsigned char)((unsigned int)(x) <= UCHAR_MAX ? (x) : (x) > 0 ? UCHAR_MAX \
                                                                                               : 0))
#define saturate_cast32s8s(x) ((char)((unsigned int)((x) - SCHAR_MIN) <= (unsigned int)UCHAR_MAX ? (x) : (x) > 0 ? SCHAR_MAX \
                                                                                                                 : SCHAR_MIN))
#define saturate_cast32s16u(x) ((unsigned short)((unsigned int)(x) <= (unsigned int)USHRT_MAX ? (x) : (x) > 0 ? USHRT_MAX \
                                                                                                              : 0))
#define saturate_cast32s16s(x) ((short)((unsigned int)((x) - SHRT_MIN) <= (unsigned int)USHRT_MAX ? (x) : (x) > 0 ? SHRT_MAX \
                                                                                                                  : SHRT_MIN))
#define saturate_cast32s32u(x) ((unsigned int)max((x), (int)0))
#define saturate_cast32s32s(x) (x)
#define saturate_cast32s64u(x) ((unsigned long)max((x), (int)0))
#define saturate_cast32s64s(x) ((long)(x))
#define saturate_cast32s32f(x) ((float)(x))
#define saturate_cast32s64f(x) ((double)(x))

////////////////////// 64u -> ... ////////////////////////

#define saturate_cast64u8u(x)  ((unsigned char)min((x), (unsigned long)UCHAR_MAX))
#define saturate_cast64u8s(x)  ((char)min((x), (unsigned long)SCHAR_MAX))
#define saturate_cast64u16u(x) ((unsigned short)min((x), (unsigned long)USHRT_MAX))
#define saturate_cast64u16s(x) ((short)min((x), (unsigned long)SHRT_MAX))
#define saturate_cast64u32u(x) ((unsigned int)min((x), (unsigned long)UINT_MAX))
#define saturate_cast64u32s(x) ((int)min((x), (unsigned long)INT_MAX))
#define saturate_cast64u64u(x) (x)
#define saturate_cast64u64s(x) ((long)min((x), (unsigned long)LLONG_MAX))
#define saturate_cast64u32f(x) ((float)(x))
#define saturate_cast64u64f(x) ((double)(x))

////////////////////// 64s -> ... ////////////////////////

#define saturate_cast64s8u(x)  ((unsigned char)((unsigned long)(x) <= (unsigned long)UCHAR_MAX ? (x) : (x) > 0 ? UCHAR_MAX \
                                                                                                               : 0))
#define saturate_cast64s8s(x) ((char)((unsigned long)((long)(x) - SCHAR_MIN) <= (unsigned long)UCHAR_MAX ? (x) : (x) > 0 ? SCHAR_MAX \
                                                                                                                         : SCHAR_MIN))
#define saturate_cast64s16u(x) ((unsigned short)((unsigned long)(x) <= (unsigned long)USHRT_MAX ? (x) : (x) > 0 ? USHRT_MAX \
                                                                                                                : 0))
#define saturate_cast64s16s(x) ((short)((unsigned long)((long)(x) - SHRT_MIN) <= (unsigned long)USHRT_MAX ? (x) : (x) > 0 ? SHRT_MAX \
                                                                                                                          : SHRT_MIN))
#define saturate_cast64s32u(x) ((unsigned int)((unsigned long)(x) <= (unsigned long)UINT_MAX ? (x) : (x) > 0 ? UINT_MAX \
                                                                                                             : 0))
#define saturate_cast64s32s(x) ((int)((unsigned long)((x) - INT_MIN) <= (unsigned long)UINT_MAX ? (x) : (x) > 0 ? INT_MAX \
                                                                                                                : INT_MIN))
#define saturate_cast64s64u(x) ((unsigned long)max((x), (long)0))
#define saturate_cast64s64s(x) (x)
#define saturate_cast64s32f(x) ((float)(x))
#define saturate_cast64s64f(x) ((double)(x))

////////////////////// 32f -> ... ////////////////////////

#define saturate_cast32f8u(x)  (saturate_cast32s8u(hlRound32f32s(x)))
#define saturate_cast32f8s(x)  (saturate_cast32s8s(hlRound32f32s(x)))
#define saturate_cast32f16u(x) (saturate_cast32s16u(hlRound32f32s(x)))
#define saturate_cast32f16s(x) (saturate_cast32s16s(hlRound32f32s(x)))
#define saturate_cast32f32u(x) ((unsigned int)hlRound32f32s(x))
#define saturate_cast32f32s(x) (hlRound32f32s(x))
#define saturate_cast32f64u(x) ((unsigned long)(x))
#define saturate_cast32f64s(x) ((long)(x))
#define saturate_cast32f32f(x) (x)
#define saturate_cast32f64f(x) ((double)(x))

////////////////////// 64f -> ... ////////////////////////

#define saturate_cast64f8u(x)  (saturate_cast32s8u(hlRound64f32s(x)))
#define saturate_cast64f8s(x)  (saturate_cast32s8s(hlRound64f32s(x)))
#define saturate_cast64f16u(x) (saturate_cast32s16u(hlRound64f32s(x)))
#define saturate_cast64f16s(x) (saturate_cast32s16s(hlRound64f32s(x)))
#define saturate_cast64f32u(x) ((unsigned int)hlRound64f32s(x))
#define saturate_cast64f32s(x) (hlRound64f32s(x))
#define saturate_cast64f64u(x) ((unsigned long)(x))
#define saturate_cast64f64s(x) ((long)(x))
#define saturate_cast64f32f(x) ((float)(x))
#define saturate_cast64f64f(x) (x)