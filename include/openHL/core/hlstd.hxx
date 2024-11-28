#pragma once

#include "openHL/core/hldef.h"
#include <cstddef>
#include <cstring>
#include <cctype>

#include <string>

#include "hlstd_wrapper.hxx"

namespace hl
{

void* fastMalloc(size_t bufSize);

void fastFree(void* ptr);

//////////////////////////////// string class ////////////////////////////////

typedef std::string String;

}    // namespace hl