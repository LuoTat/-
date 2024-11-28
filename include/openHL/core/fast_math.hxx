#pragma once

#include "openHL/core/hldef.h"
#include <cmath>

/****************************************************************************************\
*                                      fast math                                         *
\****************************************************************************************/

/** @brief Rounds floating-point number to the nearest integer

 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
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

/** @brief Rounds floating-point number to the nearest integer not larger than the original.

 The function computes an integer i such that:
 \f[i \le \texttt{value} < i+1\f]
 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
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

/** @brief Rounds floating-point number to the nearest integer not smaller than the original.

 The function computes an integer i such that:
 \f[i \le \texttt{value} < i+1\f]
 @param value floating-point number. If the value is outside of INT_MIN ... INT_MAX range, the
 result is not defined.
 */
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

/** @brief Determines if the argument is Not A Number.

 @param value The input floating-point value

 The function returns 1 if the argument is Not A Number (as defined by IEEE754 standard), 0
 otherwise. */
inline static int hlIsNaN(double value)
{
    return __builtin_isnan(value);
}

inline static int hlIsNaN(float value)
{
    return __builtin_isnan(value);
}

/** @brief Determines if the argument is Infinity.

 @param value The input floating-point value

 The function returns 1 if the argument is a plus or minus infinity (as defined by IEEE754 standard)
 and 0 otherwise. */
inline static int hlIsInf(double value)
{
    return __builtin_isinf(value);
}

inline static int hlIsInf(float value)
{
    return __builtin_isinf(value);
}