#include "precomp.hxx"

#ifdef HAVE_POSIX_MEMALIGN
    #include <stdlib.h>
#elif defined HAVE_MALLOC_H
    #include <malloc.h>
#endif

namespace hl
{
static void* OutOfMemoryError(size_t size)
{
    HL_Error_(HL_StsNoMem, ("Failed to allocate {} bytes", (unsigned long long)size));
}

void* fastMalloc(size_t size)
{
#ifdef HAVE_POSIX_MEMALIGN
    if (isAlignedAllocationEnabled())
    {
        void* ptr = NULL;
        if (posix_memalign(&ptr, HL_MALLOC_ALIGN, size))
            ptr = NULL;
        if (!ptr)
            return OutOfMemoryError(size);
        return ptr;
    }
#elif defined HAVE_MEMALIGN
    if (isAlignedAllocationEnabled())
    {
        void* ptr = memalign(HL_MALLOC_ALIGN, size);
        if (!ptr)
            return OutOfMemoryError(size);
        return ptr;
    }
#elif defined HAVE_WIN32_ALIGNED_MALLOC
    if (isAlignedAllocationEnabled())
    {
        void* ptr = _aligned_malloc(size, CV_MALLOC_ALIGN);
        if (!ptr)
            return OutOfMemoryError(size);
        return ptr;
    }
#endif
    uchar* udata = (uchar*)malloc(size + sizeof(void*) + HL_MALLOC_ALIGN);
    if (!udata)
        return OutOfMemoryError(size);
    uchar** adata = alignPtr((uchar**)udata + 1, HL_MALLOC_ALIGN);
    adata[-1]     = udata;
    return adata;
}

void fastFree(void* ptr)
{
#if defined HAVE_POSIX_MEMALIGN || defined HAVE_MEMALIGN
    if (isAlignedAllocationEnabled())
    {
        free(ptr);
        return;
    }
#elif defined HAVE_WIN32_ALIGNED_MALLOC
    if (isAlignedAllocationEnabled())
    {
        _aligned_free(ptr);
        return;
    }
#endif
    if (ptr)
    {
        uchar* udata = ((uchar**)ptr)[-1];
        HL_DbgAssert(udata < (uchar*)ptr && ((uchar*)ptr - udata) <= (ptrdiff_t)(sizeof(void*) + HL_MALLOC_ALIGN));
        free(udata);
    }
}

}    // namespace hl