#include "precomp.h"

#define DEF_SPLIT_FUNC(suffix, T)                                                         \
    static void split_##suffix(const T* src, T** dst, int len, int cn)                    \
    {                                                                                     \
        int k = cn % 4 ? cn % 4 : 4;                                                      \
        int i, j;                                                                         \
        if (k == 1)                                                                       \
        {                                                                                 \
            T* dst0 = dst[0];                                                             \
            if (cn == 1)                                                                  \
            {                                                                             \
                memcpy(dst0, src, len * sizeof(T));                                       \
            }                                                                             \
            else                                                                          \
            {                                                                             \
                for (i = 0, j = 0; i < len; i++, j += cn)                                 \
                    dst0[i] = src[j];                                                     \
            }                                                                             \
        }                                                                                 \
        else if (k == 2)                                                                  \
        {                                                                                 \
            T *dst0 = dst[0], *dst1 = dst[1];                                             \
            i = j = 0;                                                                    \
            for (; i < len; i++, j += cn)                                                 \
            {                                                                             \
                dst0[i] = src[j];                                                         \
                dst1[i] = src[j + 1];                                                     \
            }                                                                             \
        }                                                                                 \
        else if (k == 3)                                                                  \
        {                                                                                 \
            T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2];                             \
            i = j = 0;                                                                    \
            for (; i < len; i++, j += cn)                                                 \
            {                                                                             \
                dst0[i] = src[j];                                                         \
                dst1[i] = src[j + 1];                                                     \
                dst2[i] = src[j + 2];                                                     \
            }                                                                             \
        }                                                                                 \
        else                                                                              \
        {                                                                                 \
            T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2], *dst3 = dst[3];             \
            i = j = 0;                                                                    \
            for (; i < len; i++, j += cn)                                                 \
            {                                                                             \
                dst0[i] = src[j];                                                         \
                dst1[i] = src[j + 1];                                                     \
                dst2[i] = src[j + 2];                                                     \
                dst3[i] = src[j + 3];                                                     \
            }                                                                             \
        }                                                                                 \
        for (; k < cn; k += 4)                                                            \
        {                                                                                 \
            T *dst0 = dst[k], *dst1 = dst[k + 1], *dst2 = dst[k + 2], *dst3 = dst[k + 3]; \
            for (i = 0, j = k; i < len; i++, j += cn)                                     \
            {                                                                             \
                dst0[i] = src[j];                                                         \
                dst1[i] = src[j + 1];                                                     \
                dst2[i] = src[j + 2];                                                     \
                dst3[i] = src[j + 3];                                                     \
            }                                                                             \
        }                                                                                 \
    }

DEF_SPLIT_FUNC(8, unsigned char)
DEF_SPLIT_FUNC(16, unsigned short)
DEF_SPLIT_FUNC(32, unsigned int)
DEF_SPLIT_FUNC(64, unsigned long)

void split8_cpu_baseline(const unsigned char* src, unsigned char** dst, int len, int cn)
{
    split_8(src, dst, len, cn);
}

void split16_cpu_baseline(const unsigned short* src, unsigned short** dst, int len, int cn)
{
    split_16(src, dst, len, cn);
}

void split32_cpu_baseline(const unsigned int* src, unsigned int** dst, int len, int cn)
{
    split_32(src, dst, len, cn);
}

void split64_cpu_baseline(const unsigned long* src, unsigned long** dst, int len, int cn)
{
    split_64(src, dst, len, cn);
}