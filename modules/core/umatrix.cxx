#include "precomp.hxx"

namespace hl
{

UMatData::UMatData(const MatAllocator* allocator)
{
    prevAllocator = currAllocator = allocator;
    urefcount = refcount = mapcount = 0;
    data = origdata  = 0;
    size             = 0;
    flags            = static_cast<UMatData::MemoryFlag>(0);
    handle           = 0;
    userdata         = 0;
    allocatorFlags_  = 0;
    originalUMatData = NULL;
}

UMatData::~UMatData()
{
    prevAllocator = currAllocator = 0;
    urefcount = refcount = 0;
    HL_Assert(mapcount == 0);
    data = origdata     = 0;
    size                = 0;
    bool isAsyncCleanup = !!(flags & UMatData::ASYNC_CLEANUP);
    flags               = static_cast<UMatData::MemoryFlag>(0);
    handle              = 0;
    userdata            = 0;
    allocatorFlags_     = 0;
    if (originalUMatData)
    {
        bool      showWarn = false;
        UMatData* u        = originalUMatData;
        bool      zero_Ref = HL_XADD(&(u->refcount), -1) == 1;
        if (zero_Ref)
        {
            // simulate Mat::deallocate
            if (u->mapcount != 0)
            {
                (u->currAllocator ? u->currAllocator : /* TODO allocator ? allocator :*/ Mat::getDefaultAllocator())->unmap(u);
            }
            else
            {
                // we don't do "map", so we can't do "unmap"
            }
        }
        bool zero_URef = HL_XADD(&(u->urefcount), -1) == 1;
        if (zero_Ref && !zero_URef)
            showWarn = true;
        if (zero_Ref && zero_URef)    // oops, we need to free resources
        {
            showWarn = !isAsyncCleanup;
            // simulate UMat::deallocate
            u->currAllocator->deallocate(u);
        }
#ifndef NDEBUG
        if (showWarn)
        {
            static int warn_message_showed = 0;
            if (warn_message_showed++ < 100)
            {
                fflush(stdout);
                fprintf(stderr, "\n! OPENHL warning: getUMat()/getMat() call chain possible problem."
                                "\n!                 Base object is dead, while nested/derived object is still alive or processed."
                                "\n!                 Please check lifetime of UMat/Mat objects!\n");
                fflush(stderr);
            }
        }
#else
        HL_UNUSED(showWarn);
#endif
        originalUMatData = NULL;
    }
}

enum
{
    UMAT_NLOCKS = 31
};

static Mutex umatLocks[UMAT_NLOCKS];

static size_t getUMatDataLockIndex(const UMatData* u)
{
    size_t idx = ((size_t)(void*)u) % UMAT_NLOCKS;
    return idx;
}

void UMatData::lock()
{
    size_t idx = getUMatDataLockIndex(this);
    umatLocks[idx].lock();
}

void UMatData::unlock()
{
    size_t idx = getUMatDataLockIndex(this);
    umatLocks[idx].unlock();
}

}    // namespace hl