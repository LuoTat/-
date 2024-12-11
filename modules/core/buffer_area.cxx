#include "openHL/core/utils/buffer_area.private.hxx"

static bool HL_BUFFER_AREA_OVERRIDE_SAFE_MODE = false;

namespace hl
{
namespace utils
{

//==================================================================================================

class BufferArea::Block
{
private:
    inline size_t reserve_count() const
    {
        return alignment / type_size - 1;
    }

public:
    Block(void** ptr_, ushort type_size_, size_t count_, ushort alignment_):
        ptr(ptr_), raw_mem(0), count(count_), type_size(type_size_), alignment(alignment_)
    {
        HL_Assert(ptr && *ptr == NULL);
    }

    void cleanup() const
    {
        HL_DbgAssert(ptr);
        if (raw_mem)
            fastFree(raw_mem);
    }

    size_t getByteCount() const
    {
        return type_size * (count + reserve_count());
    }

    void real_allocate()
    {
        HL_Assert(ptr && *ptr == NULL);
        const size_t allocated_count = count + reserve_count();
        raw_mem                      = fastMalloc(type_size * allocated_count);
        if (alignment != type_size)
        {
            *ptr = alignPtr(raw_mem, alignment);
            HL_Assert(reinterpret_cast<size_t>(*ptr) % alignment == 0);
            HL_Assert(static_cast<uchar*>(*ptr) + type_size * count <= static_cast<uchar*>(raw_mem) + type_size * allocated_count);
        }
        else
        {
            *ptr = raw_mem;
        }
    }

    void* fast_allocate(void* buf) const
    {
        HL_Assert(ptr && *ptr == NULL);
        buf = alignPtr(buf, alignment);
        HL_Assert(reinterpret_cast<size_t>(buf) % alignment == 0);
        *ptr = buf;
        return static_cast<void*>(static_cast<uchar*>(*ptr) + type_size * count);
    }

    bool operator==(void** other) const
    {
        HL_Assert(ptr && other);
        return *ptr == *other;
    }

    void zeroFill() const
    {
        HL_Assert(ptr && *ptr);
        memset(static_cast<uchar*>(*ptr), 0, count * type_size);
    }

private:
    void** ptr;
    void*  raw_mem;
    size_t count;
    ushort type_size;
    ushort alignment;
};

//==================================================================================================

BufferArea::BufferArea(bool safe_):
    oneBuf(0),
    totalSize(0),
    safe(safe_ || HL_BUFFER_AREA_OVERRIDE_SAFE_MODE)
{
    // nothing
}

BufferArea::~BufferArea()
{
    release();
}

void BufferArea::allocate_(void** ptr, ushort type_size, size_t count, ushort alignment)
{
    blocks.push_back(Block(ptr, type_size, count, alignment));
    if (!safe)
    {
        totalSize += blocks.back().getByteCount();
    }
    else
    {
        blocks.back().real_allocate();
    }
}

void BufferArea::zeroFill_(void** ptr)
{
    for (std::vector<Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
    {
        if (*i == ptr)
        {
            i->zeroFill();
            break;
        }
    }
}

void BufferArea::zeroFill()
{
    for (std::vector<Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
    {
        i->zeroFill();
    }
}

void BufferArea::commit()
{
    if (!safe)
    {
        HL_Assert(totalSize > 0);
        HL_Assert(oneBuf == NULL);
        HL_Assert(!blocks.empty());
        oneBuf    = fastMalloc(totalSize);
        void* ptr = oneBuf;
        for (std::vector<Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
        {
            ptr = i->fast_allocate(ptr);
        }
    }
}

void BufferArea::release()
{
    for (std::vector<Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
    {
        i->cleanup();
    }
    blocks.clear();
    if (oneBuf)
    {
        fastFree(oneBuf);
        oneBuf = 0;
    }
}

//==================================================================================================

}    // namespace utils
}    // namespace hl
