#pragma once

#include <cstddef>
#include <cstdint>

typedef signed char    schar;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef int64_t        int64;
typedef uint64_t       uint64;
#define HL_BIG_INT(n)          n##LL
#define HL_BIG_UINT(n)         n##ULL

#define HL_MAX_DIM             32
#define HL_CN_MAX              512
#define HL_CN_SHIFT            3
#define HL_DEPTH_MAX           (1 << HL_CN_SHIFT)

#define HL_8U                  0
#define HL_8S                  1
#define HL_16U                 2
#define HL_16S                 3
#define HL_32U                 4
#define HL_32S                 5
#define HL_32F                 6
#define HL_64F                 7

#define HL_MAT_DEPTH_MASK      (HL_DEPTH_MAX - 1)

#define HL_MAT_DEPTH(flags)    ((flags) & HL_MAT_DEPTH_MASK)
#define HL_MAKETYPE(depth, cn) (HL_MAT_DEPTH(depth) + (((cn) - 1) << HL_CN_SHIFT))
#define HL_MAKE_TYPE           HL_MAKETYPE

#define HL_8UC1                HL_MAKETYPE(HL_8U, 1)
#define HL_8UC2                HL_MAKETYPE(HL_8U, 2)
#define HL_8UC3                HL_MAKETYPE(HL_8U, 3)
#define HL_8UC4                HL_MAKETYPE(HL_8U, 4)

#define HL_8SC1                HL_MAKETYPE(HL_8S, 1)
#define HL_8SC2                HL_MAKETYPE(HL_8S, 2)
#define HL_8SC3                HL_MAKETYPE(HL_8S, 3)
#define HL_8SC4                HL_MAKETYPE(HL_8S, 4)

#define HL_16UC1               HL_MAKETYPE(HL_16U, 1)
#define HL_16UC2               HL_MAKETYPE(HL_16U, 2)
#define HL_16UC3               HL_MAKETYPE(HL_16U, 3)
#define HL_16UC4               HL_MAKETYPE(HL_16U, 4)

#define HL_16SC1               HL_MAKETYPE(HL_16S, 1)
#define HL_16SC2               HL_MAKETYPE(HL_16S, 2)
#define HL_16SC3               HL_MAKETYPE(HL_16S, 3)
#define HL_16SC4               HL_MAKETYPE(HL_16S, 4)

#define HL_32UC1               HL_MAKETYPE(HL_32U, 1)
#define HL_32UC2               HL_MAKETYPE(HL_32U, 2)
#define HL_32UC3               HL_MAKETYPE(HL_32U, 3)
#define HL_32UC4               HL_MAKETYPE(HL_32U, 4)

#define HL_32SC1               HL_MAKETYPE(HL_32S, 1)
#define HL_32SC2               HL_MAKETYPE(HL_32S, 2)
#define HL_32SC3               HL_MAKETYPE(HL_32S, 3)
#define HL_32SC4               HL_MAKETYPE(HL_32S, 4)

#define HL_32FC1               HL_MAKETYPE(HL_32F, 1)
#define HL_32FC2               HL_MAKETYPE(HL_32F, 2)
#define HL_32FC3               HL_MAKETYPE(HL_32F, 3)
#define HL_32FC4               HL_MAKETYPE(HL_32F, 4)

#define HL_64FC1               HL_MAKETYPE(HL_64F, 1)
#define HL_64FC2               HL_MAKETYPE(HL_64F, 2)
#define HL_64FC3               HL_MAKETYPE(HL_64F, 3)
#define HL_64FC4               HL_MAKETYPE(HL_64F, 4)