#pragma once

#include "openHL/core/hldef.h"
#include <math.h>

#define min(a, b) ((a) < (b) ? (a) : (b))

#define max(a, b) ((a) > (b) ? (a) : (b))

inline static int hlRound32f32s(double value)
{
    return (int)lrint(value);
}

inline static int hlRound64f32s(double value)
{
    return (int)lrint(value);
}
