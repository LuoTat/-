#pragma once

#include "openHL/core/bufferpool.hxx"

namespace hl
{
class DummyBufferPoolController: public BufferPoolController
{
public:
    DummyBufferPoolController() {}

    virtual ~DummyBufferPoolController() {}

    virtual size_t getReservedSize() const override { return (size_t)-1; }

    virtual size_t getMaxReservedSize() const override { return (size_t)-1; }

    virtual void setMaxReservedSize(size_t size) override { HL_UNUSED(size); }

    virtual void freeAllReservedBuffers() override {}
};
}    // namespace hl