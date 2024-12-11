#include "precomp.hxx"
#include "split.simd.hxx"

namespace hl
{
namespace hal
{

void split8u(const uchar* src, uchar** dst, int len, int cn)
{
    return cpu_baseline::split8u(src, dst, len, cn);
}

void split16u(const ushort* src, ushort** dst, int len, int cn)
{
    return cpu_baseline::split16u(src, dst, len, cn);
}

void split32s(const int* src, int** dst, int len, int cn)
{
    return cpu_baseline::split32s(src, dst, len, cn);
}

void split64s(const int64* src, int64** dst, int len, int cn)
{
    return cpu_baseline::split64s(src, dst, len, cn);
}

}    // namespace hal

/****************************************************************************************\
*                                       split & merge                                    *
\****************************************************************************************/

typedef void (*SplitFunc)(const uchar* src, uchar** dst, int len, int cn);

static SplitFunc getSplitFunc(int depth)
{
    static SplitFunc splitTab[] = {
        (SplitFunc)hl::hal::split8u, (SplitFunc)hl::hal::split8u, (SplitFunc)hl::hal::split16u, (SplitFunc)hl::hal::split16u, (SplitFunc)hl::hal::split32s, (SplitFunc)hl::hal::split32s, (SplitFunc)hl::hal::split64s, (SplitFunc)hl::hal::split16u};

    return splitTab[depth];
}

void split(const Mat& src, Mat* mv)
{
    int k, depth = src.depth(), cn = src.channels();
    if (cn == 1)
    {
        src.copyTo(mv[0]);
        return;
    }

    for (k = 0; k < cn; k++)
    {
        mv[k].create(src.dims, src.size, depth);
    }

    SplitFunc func = getSplitFunc(depth);
    HL_Assert(func != 0);

    size_t            esz = src.elemSize(), esz1 = src.elemSize1();
    size_t            blocksize0 = (BLOCK_SIZE + esz - 1) / esz;
    AutoBuffer<uchar> _buf((cn + 1) * (sizeof(Mat*) + sizeof(uchar*)) + 16);
    const Mat**       arrays = (const Mat**)_buf.data();
    uchar**           ptrs   = (uchar**)alignPtr(arrays + cn + 1, 16);

    arrays[0]                = &src;
    for (k = 0; k < cn; k++)
    {
        arrays[k + 1] = &mv[k];
    }

    NAryMatIterator it(arrays, ptrs, cn + 1);
    size_t          total     = it.size;
    size_t          blocksize = std::min((size_t)HL_SPLIT_MERGE_MAX_BLOCK_SIZE(cn), cn <= 4 ? total : std::min(total, blocksize0));

    for (size_t i = 0; i < it.nplanes; i++, ++it)
    {
        for (size_t j = 0; j < total; j += blocksize)
        {
            size_t bsz = std::min(total - j, blocksize);
            func(ptrs[0], &ptrs[1], (int)bsz, cn);

            if (j + blocksize < total)
            {
                ptrs[0] += bsz * esz;
                for (k = 0; k < cn; k++)
                    ptrs[k + 1] += bsz * esz1;
            }
        }
    }
}

}    // namespace hl