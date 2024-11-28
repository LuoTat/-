#pragma once

#include "openHL/core/hldef.h"
#include <climits>
#include "openHL/core/fast_math.hxx"

namespace hl
{

template <typename _Tp> inline static _Tp saturate_cast(uchar v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(schar v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(ushort v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(short v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(uint v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(int v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(float v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(double v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(int64 v) { return _Tp(v); }

template <typename _Tp> inline static _Tp saturate_cast(uint64 v) { return _Tp(v); }

template <> inline uchar saturate_cast<uchar>(schar v) { return (uchar)std::max((int)v, 0); }

template <> inline uchar saturate_cast<uchar>(ushort v) { return (uchar)std::min((uint)v, (uint)UCHAR_MAX); }

template <> inline uchar saturate_cast<uchar>(int v) { return (uchar)((uint)v <= UCHAR_MAX ? v : v > 0 ? UCHAR_MAX
                                                                                                       : 0); }

template <> inline uchar saturate_cast<uchar>(short v) { return saturate_cast<uchar>((int)v); }

template <> inline uchar saturate_cast<uchar>(uint v) { return (uchar)std::min(v, (uint)UCHAR_MAX); }

template <> inline uchar saturate_cast<uchar>(float v)
{
    int iv = hlRound(v);
    return saturate_cast<uchar>(iv);
}

template <> inline uchar saturate_cast<uchar>(double v)
{
    int iv = hlRound(v);
    return saturate_cast<uchar>(iv);
}

template <> inline uchar saturate_cast<uchar>(int64 v) { return (uchar)((uint64)v <= (uint64)UCHAR_MAX ? v : v > 0 ? UCHAR_MAX
                                                                                                                   : 0); }

template <> inline uchar saturate_cast<uchar>(uint64 v) { return (uchar)std::min(v, (uint64)UCHAR_MAX); }

template <> inline schar saturate_cast<schar>(uchar v) { return (schar)std::min((int)v, SCHAR_MAX); }

template <> inline schar saturate_cast<schar>(ushort v) { return (schar)std::min((uint)v, (uint)SCHAR_MAX); }

template <> inline schar saturate_cast<schar>(int v) { return (schar)((uint)(v - SCHAR_MIN) <= (uint)UCHAR_MAX ? v : v > 0 ? SCHAR_MAX
                                                                                                                           : SCHAR_MIN); }

template <> inline schar saturate_cast<schar>(short v) { return saturate_cast<schar>((int)v); }

template <> inline schar saturate_cast<schar>(uint v) { return (schar)std::min(v, (uint)SCHAR_MAX); }

template <> inline schar saturate_cast<schar>(float v)
{
    int iv = hlRound(v);
    return saturate_cast<schar>(iv);
}

template <> inline schar saturate_cast<schar>(double v)
{
    int iv = hlRound(v);
    return saturate_cast<schar>(iv);
}

template <> inline schar saturate_cast<schar>(int64 v) { return (schar)((uint64)((int64)v - SCHAR_MIN) <= (uint64)UCHAR_MAX ? v : v > 0 ? SCHAR_MAX
                                                                                                                                        : SCHAR_MIN); }

template <> inline schar saturate_cast<schar>(uint64 v) { return (schar)std::min(v, (uint64)SCHAR_MAX); }

template <> inline ushort saturate_cast<ushort>(schar v) { return (ushort)std::max((int)v, 0); }

template <> inline ushort saturate_cast<ushort>(short v) { return (ushort)std::max((int)v, 0); }

template <> inline ushort saturate_cast<ushort>(int v) { return (ushort)((uint)v <= (uint)USHRT_MAX ? v : v > 0 ? USHRT_MAX
                                                                                                                : 0); }

template <> inline ushort saturate_cast<ushort>(uint v) { return (ushort)std::min(v, (uint)USHRT_MAX); }

template <> inline ushort saturate_cast<ushort>(float v)
{
    int iv = hlRound(v);
    return saturate_cast<ushort>(iv);
}

template <> inline ushort saturate_cast<ushort>(double v)
{
    int iv = hlRound(v);
    return saturate_cast<ushort>(iv);
}

template <> inline ushort saturate_cast<ushort>(int64 v) { return (ushort)((uint64)v <= (uint64)USHRT_MAX ? v : v > 0 ? USHRT_MAX
                                                                                                                      : 0); }

template <> inline ushort saturate_cast<ushort>(uint64 v) { return (ushort)std::min(v, (uint64)USHRT_MAX); }

template <> inline short saturate_cast<short>(ushort v) { return (short)std::min((int)v, SHRT_MAX); }

template <> inline short saturate_cast<short>(int v) { return (short)((uint)(v - SHRT_MIN) <= (uint)USHRT_MAX ? v : v > 0 ? SHRT_MAX
                                                                                                                          : SHRT_MIN); }

template <> inline short saturate_cast<short>(uint v) { return (short)std::min(v, (uint)SHRT_MAX); }

template <> inline short saturate_cast<short>(float v)
{
    int iv = hlRound(v);
    return saturate_cast<short>(iv);
}

template <> inline short saturate_cast<short>(double v)
{
    int iv = hlRound(v);
    return saturate_cast<short>(iv);
}

template <> inline short saturate_cast<short>(int64 v) { return (short)((uint64)((int64)v - SHRT_MIN) <= (uint64)USHRT_MAX ? v : v > 0 ? SHRT_MAX
                                                                                                                                       : SHRT_MIN); }

template <> inline short saturate_cast<short>(uint64 v) { return (short)std::min(v, (uint64)SHRT_MAX); }

template <> inline int saturate_cast<int>(uint v) { return (int)std::min(v, (uint)INT_MAX); }

template <> inline int saturate_cast<int>(int64 v) { return (int)((uint64)(v - INT_MIN) <= (uint64)UINT_MAX ? v : v > 0 ? INT_MAX
                                                                                                                        : INT_MIN); }

template <> inline int saturate_cast<int>(uint64 v) { return (int)std::min(v, (uint64)INT_MAX); }

template <> inline int saturate_cast<int>(float v) { return hlRound(v); }

template <> inline int saturate_cast<int>(double v) { return hlRound(v); }

template <> inline uint saturate_cast<uint>(schar v) { return (uint)std::max(v, (schar)0); }

template <> inline uint saturate_cast<uint>(short v) { return (uint)std::max(v, (short)0); }

template <> inline uint saturate_cast<uint>(int v) { return (uint)std::max(v, (int)0); }

template <> inline uint saturate_cast<uint>(int64 v) { return (uint)((uint64)v <= (uint64)UINT_MAX ? v : v > 0 ? UINT_MAX
                                                                                                               : 0); }

template <> inline uint saturate_cast<uint>(uint64 v) { return (uint)std::min(v, (uint64)UINT_MAX); }

// we intentionally do not clip negative numbers, to make -1 become 0xffffffff etc.
template <> inline uint saturate_cast<uint>(float v) { return static_cast<uint>(hlRound(v)); }

template <> inline uint saturate_cast<uint>(double v) { return static_cast<uint>(hlRound(v)); }

template <> inline uint64 saturate_cast<uint64>(schar v) { return (uint64)std::max(v, (schar)0); }

template <> inline uint64 saturate_cast<uint64>(short v) { return (uint64)std::max(v, (short)0); }

template <> inline uint64 saturate_cast<uint64>(int v) { return (uint64)std::max(v, (int)0); }

template <> inline uint64 saturate_cast<uint64>(int64 v) { return (uint64)std::max(v, (int64)0); }

template <> inline int64 saturate_cast<int64>(uint64 v) { return (int64)std::min(v, (uint64)LLONG_MAX); }
}    // namespace hl
