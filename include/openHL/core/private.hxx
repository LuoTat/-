#pragma once

#include "openHL/core.hxx"

#define HL_MALLOC_ALIGN 64

namespace hl
{

void scalarToRawData(const hl::Scalar& s, void* buf, int type, int unroll_to = 0);

}    // namespace hl
