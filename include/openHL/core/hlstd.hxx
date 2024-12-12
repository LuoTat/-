#pragma once

#include "openHL/core/hldef.h"
#include <cstddef>
#include <cstring>
#include <cctype>

#include <string>

// import useful primitives from stl
#include <algorithm>
#include <utility>
#include <cstdlib>    //for abs(int)
#include <cmath>

namespace hl
{

inline static uchar abs(uchar a) { return a; }

inline static ushort abs(ushort a) { return a; }

inline static unsigned abs(unsigned a) { return a; }

inline static uint64 abs(uint64 a) { return a; }

using std::abs;
using std::exp;
using std::log;
using std::max;
using std::min;
using std::pow;
using std::sqrt;
using std::swap;

}    // namespace hl

#include "hlstd_wrapper.hxx"

namespace hl
{

void* fastMalloc(size_t bufSize);

void fastFree(void* ptr);

//////////////////////////////// string class ////////////////////////////////

typedef std::string String;

}    // namespace hl