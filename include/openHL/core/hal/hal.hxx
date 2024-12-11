#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.hxx"
#include "openHL/core/hal/interface.h"

namespace hl
{
namespace hal
{

void split8u(const uchar* src, uchar** dst, int len, int cn);
void split16u(const ushort* src, ushort** dst, int len, int cn);
void split32s(const int* src, int** dst, int len, int cn);
void split64s(const int64* src, int64** dst, int len, int cn);

}    // namespace hal
}    // namespace hl