#pragma once

#include "openHL/core/hldef.h"
#include <cmath>

/****************************************************************************************\
*                                      fast math                                         *
\****************************************************************************************/

inline static int hlRound(double value)
{
    return (int)__builtin_lrint(value);
}

inline static int hlRound(float value)
{
    return (int)__builtin_lrintf(value);
}

inline static int hlRound(int value)
{
    return value;
}

inline static int hlFloor(double value)
{
    return (int)__builtin_floor(value);
}

inline static int hlFloor(float value)
{
    return (int)__builtin_floorf(value);
}

inline static int hlFloor(int value)
{
    return value;
}

inline static int hlCeil(double value)
{
    return (int)__builtin_ceil(value);
}

inline static int hlCeil(float value)
{
    return (int)__builtin_ceilf(value);
}

inline static int hlCeil(int value)
{
    return value;
}

inline static int hlIsNaN(double value)
{
    return __builtin_isnan(value);
}

inline static int hlIsNaN(float value)
{
    return __builtin_isnan(value);
}

inline static int hlIsInf(double value)
{
    return __builtin_isinf(value);
}

inline static int hlIsInf(float value)
{
    return __builtin_isinf(value);
}