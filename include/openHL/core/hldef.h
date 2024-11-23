#pragma once

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

// 获得depth
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

#define HL_16FC1               HL_MAKETYPE(HL_16F, 1)
#define HL_16FC2               HL_MAKETYPE(HL_16F, 2)
#define HL_16FC3               HL_MAKETYPE(HL_16F, 3)
#define HL_16FC4               HL_MAKETYPE(HL_16F, 4)



#define HL_MAT_CN_MASK         ((HL_CN_MAX - 1) << HL_CN_SHIFT)
// 获得通道数
#define HL_MAT_CN(flags)       ((((flags) & HL_MAT_CN_MASK) >> HL_CN_SHIFT) + 1)
#define HL_MAT_TYPE_MASK       (HL_DEPTH_MAX * HL_CN_MAX - 1)
// 获得type
#define HL_MAT_TYPE(flags)     ((flags) & HL_MAT_TYPE_MASK)
/** Size of each channel item,
   0x28442211 = 0010 1000 0100 0100 0010 0010 0001 0001 ~ array of sizeof(arr_type_elem) */
#define HL_ELEM_SIZE1(type)    ((0x28442211 >> HL_MAT_DEPTH(type) * 4) & 15)
// 获得元素的字节数
#define HL_ELEM_SIZE(type)     (HL_MAT_CN(type) * HL_ELEM_SIZE1(type))