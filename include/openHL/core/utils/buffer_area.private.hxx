#pragma once

#include <openHL/core/base.hxx>
#include <openHL/core/private.hxx>
#include <openHL/core/utility.hxx>
#include <vector>

namespace hl
{
namespace utils
{

class BufferArea
{
public:
    BufferArea(bool safe = false);

    ~BufferArea();

    template <typename T>
    void allocate(T*& ptr, size_t count, ushort alignment = sizeof(T))
    {
        HL_Assert(ptr == NULL);
        HL_Assert(count > 0);
        HL_Assert(alignment > 0);
        HL_Assert(alignment % sizeof(T) == 0);
        HL_Assert((alignment & (alignment - 1)) == 0);
        allocate_((void**)(&ptr), static_cast<ushort>(sizeof(T)), count, alignment);
        if (safe)
            HL_Assert(ptr != NULL);
    }

    template <typename T>
    void zeroFill(T*& ptr)
    {
        HL_Assert(ptr);
        zeroFill_((void**)&ptr);
    }

    void zeroFill();

    void commit();

    void release();

private:
    BufferArea(const BufferArea&);               // = delete
    BufferArea& operator=(const BufferArea&);    // = delete
    void        allocate_(void** ptr, ushort type_size, size_t count, ushort alignment);
    void        zeroFill_(void** ptr);

private:
    class Block;
    std::vector<Block> blocks;
    void*              oneBuf;
    size_t             totalSize;
    const bool         safe;
};

}    // namespace utils
}    // namespace hl
