#include "precomp.hxx"

namespace hl
{
namespace hal
{
namespace cpu_baseline
{

void split8u(const uchar* src, uchar** dst, int len, int cn);
void split16u(const ushort* src, ushort** dst, int len, int cn);
void split32s(const int* src, int** dst, int len, int cn);
void split64s(const int64* src, int64** dst, int len, int cn);

template <typename T>
static void split_(const T* src, T** dst, int len, int cn)
{
    int k = cn % 4 ? cn % 4 : 4;
    int i, j;
    if (k == 1)
    {
        T* dst0 = dst[0];

        if (cn == 1)
        {
            memcpy(dst0, src, len * sizeof(T));
        }
        else
        {
            for (i = 0, j = 0; i < len; i++, j += cn)
                dst0[i] = src[j];
        }
    }
    else if (k == 2)
    {
        T *dst0 = dst[0], *dst1 = dst[1];
        i = j = 0;

        for (; i < len; i++, j += cn)
        {
            dst0[i] = src[j];
            dst1[i] = src[j + 1];
        }
    }
    else if (k == 3)
    {
        T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2];
        i = j = 0;

        for (; i < len; i++, j += cn)
        {
            dst0[i] = src[j];
            dst1[i] = src[j + 1];
            dst2[i] = src[j + 2];
        }
    }
    else
    {
        T *dst0 = dst[0], *dst1 = dst[1], *dst2 = dst[2], *dst3 = dst[3];
        i = j = 0;

        for (; i < len; i++, j += cn)
        {
            dst0[i] = src[j];
            dst1[i] = src[j + 1];
            dst2[i] = src[j + 2];
            dst3[i] = src[j + 3];
        }
    }

    for (; k < cn; k += 4)
    {
        T *dst0 = dst[k], *dst1 = dst[k + 1], *dst2 = dst[k + 2], *dst3 = dst[k + 3];
        for (i = 0, j = k; i < len; i++, j += cn)
        {
            dst0[i] = src[j];
            dst1[i] = src[j + 1];
            dst2[i] = src[j + 2];
            dst3[i] = src[j + 3];
        }
    }
}

void split8u(const uchar* src, uchar** dst, int len, int cn)
{
    split_(src, dst, len, cn);
}

void split16u(const ushort* src, ushort** dst, int len, int cn)
{
    split_(src, dst, len, cn);
}

void split32s(const int* src, int** dst, int len, int cn)
{
    split_(src, dst, len, cn);
}

void split64s(const int64* src, int64** dst, int len, int cn)
{
    split_(src, dst, len, cn);
}

}    // namespace cpu_baseline
}    // namespace hal
}    // namespace hl