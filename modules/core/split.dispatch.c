#include "precomp.h"
#include "split.simd.h"

void split8(const void* src, void** dst, int len, int cn)
{
    split8_cpu_baseline((const unsigned char*)src, (unsigned char**)dst, len, cn);
}

void split16(const void* src, void** dst, int len, int cn)
{
    split16_cpu_baseline((const unsigned short*)src, (unsigned short**)dst, len, cn);
}

void split32(const void* src, void** dst, int len, int cn)
{
    split32_cpu_baseline((const unsigned int*)src, (unsigned int**)dst, len, cn);
}

void split64(const void* src, void** dst, int len, int cn)
{
    split64_cpu_baseline((unsigned long*)src, (unsigned long**)dst, len, cn);
}

typedef void (*SplitFunc)(const void* src, void** dst, int len, int cn);

static SplitFunc getSplitFunc(int depth)
{
    static SplitFunc splitTab[] = {
        split8, split8, split16, split16, split32, split32, split32, split64};

    return splitTab[depth];
}

void split(const Mat* src, Mat* mvbegin)
{
    int k, sdepth = depth(src), cn = channels(src);
    if (cn == 1)
    {
        copyTo(src, &mvbegin[0]);
        return;
    }

    for (k = 0; k < cn; k++)
    {
        create(&mvbegin[k], src->rows, src->cols, sdepth);
    }

    SplitFunc func = getSplitFunc(sdepth);
    assert(func != 0);

    unsigned char** ptrs = (unsigned char**)alloca(cn * sizeof(unsigned char*));
    ptrs[0]              = src->data;
    for (k = 0; k < cn; ++k)
    {
        ptrs[k + 1] = mvbegin[k].data;
    }

    func((const void*)ptrs[0], (void**)&ptrs[1], src->cols * src->rows, cn);
}