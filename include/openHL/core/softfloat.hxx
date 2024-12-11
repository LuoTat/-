#pragma once

#include "hldef.h"

namespace hl
{

struct softfloat;
struct softdouble;

struct softfloat
{
public:
    softfloat() { v = 0; }

    softfloat(const softfloat& c) { v = c.v; }

    softfloat& operator=(const softfloat& c)
    {
        if (&c != this) v = c.v;
        return *this;
    }

    const static softfloat fromRaw(const uint32_t a)
    {
        softfloat x;
        x.v = a;
        return x;
    }

    explicit softfloat(const uint32_t);
    explicit softfloat(const uint64_t);
    explicit softfloat(const int32_t);
    explicit softfloat(const int64_t);

    explicit softfloat(const float a)
    {
        Hl32suf s;
        s.f = a;
        v   = s.u;
    }

    operator softdouble() const;

    operator float() const
    {
        Hl32suf s;
        s.u = v;
        return s.f;
    }

    softfloat operator+(const softfloat&) const;
    softfloat operator-(const softfloat&) const;
    softfloat operator*(const softfloat&) const;
    softfloat operator/(const softfloat&) const;

    softfloat operator-() const
    {
        softfloat x;
        x.v = v ^ (1U << 31);
        return x;
    }

    softfloat operator%(const softfloat&) const;

    softfloat& operator+=(const softfloat& a)
    {
        *this = *this + a;
        return *this;
    }

    softfloat& operator-=(const softfloat& a)
    {
        *this = *this - a;
        return *this;
    }

    softfloat& operator*=(const softfloat& a)
    {
        *this = *this * a;
        return *this;
    }

    softfloat& operator/=(const softfloat& a)
    {
        *this = *this / a;
        return *this;
    }

    softfloat& operator%=(const softfloat& a)
    {
        *this = *this % a;
        return *this;
    }

    bool operator==(const softfloat&) const;
    bool operator!=(const softfloat&) const;
    bool operator>(const softfloat&) const;
    bool operator>=(const softfloat&) const;
    bool operator<(const softfloat&) const;
    bool operator<=(const softfloat&) const;

    inline bool isNaN() const { return (v & 0x7fffffff) > 0x7f800000; }

    inline bool isInf() const { return (v & 0x7fffffff) == 0x7f800000; }

    inline bool isSubnormal() const { return ((v >> 23) & 0xFF) == 0; }

    inline bool getSign() const { return (v >> 31) != 0; }

    inline softfloat setSign(bool sign) const
    {
        softfloat x;
        x.v = (v & ((1U << 31) - 1)) | ((uint32_t)sign << 31);
        return x;
    }

    inline int getExp() const { return ((v >> 23) & 0xFF) - 127; }

    inline softfloat setExp(int e) const
    {
        softfloat x;
        x.v = (v & 0x807fffff) | (((e + 127) & 0xFF) << 23);
        return x;
    }

    inline softfloat getFrac() const
    {
        uint_fast32_t vv = (v & 0x007fffff) | (127 << 23);
        return softfloat::fromRaw(vv);
    }

    inline softfloat setFrac(const softfloat& s) const
    {
        softfloat x;
        x.v = (v & 0xff800000) | (s.v & 0x007fffff);
        return x;
    }

    static softfloat zero() { return softfloat::fromRaw(0); }

    static softfloat inf() { return softfloat::fromRaw(0xFF << 23); }

    static softfloat nan() { return softfloat::fromRaw(0x7fffffff); }

    static softfloat one() { return softfloat::fromRaw(127 << 23); }

    static softfloat min() { return softfloat::fromRaw(0x01 << 23); }

    static softfloat eps() { return softfloat::fromRaw((127 - 23) << 23); }

    static softfloat max() { return softfloat::fromRaw((0xFF << 23) - 1); }

    static softfloat pi() { return softfloat::fromRaw(0x40490fdb); }

    uint32_t v;
};

/*----------------------------------------------------------------------------
*----------------------------------------------------------------------------*/

struct softdouble
{
public:
    softdouble():
        v(0) {}

    softdouble(const softdouble& c) { v = c.v; }

    softdouble& operator=(const softdouble& c)
    {
        if (&c != this) v = c.v;
        return *this;
    }

    static softdouble fromRaw(const uint64_t a)
    {
        softdouble x;
        x.v = a;
        return x;
    }

    explicit softdouble(const uint32_t);
    explicit softdouble(const uint64_t);
    explicit softdouble(const int32_t);
    explicit softdouble(const int64_t);

    explicit softdouble(const double a)
    {
        Hl64suf s;
        s.f = a;
        v   = s.u;
    }

    operator softfloat() const;

    operator double() const
    {
        Hl64suf s;
        s.u = v;
        return s.f;
    }

    softdouble operator+(const softdouble&) const;
    softdouble operator-(const softdouble&) const;
    softdouble operator*(const softdouble&) const;
    softdouble operator/(const softdouble&) const;

    softdouble operator-() const
    {
        softdouble x;
        x.v = v ^ (1ULL << 63);
        return x;
    }

    softdouble operator%(const softdouble&) const;

    softdouble& operator+=(const softdouble& a)
    {
        *this = *this + a;
        return *this;
    }

    softdouble& operator-=(const softdouble& a)
    {
        *this = *this - a;
        return *this;
    }

    softdouble& operator*=(const softdouble& a)
    {
        *this = *this * a;
        return *this;
    }

    softdouble& operator/=(const softdouble& a)
    {
        *this = *this / a;
        return *this;
    }

    softdouble& operator%=(const softdouble& a)
    {
        *this = *this % a;
        return *this;
    }

    bool operator==(const softdouble&) const;
    bool operator!=(const softdouble&) const;
    bool operator>(const softdouble&) const;
    bool operator>=(const softdouble&) const;
    bool operator<(const softdouble&) const;
    bool operator<=(const softdouble&) const;

    inline bool isNaN() const { return (v & 0x7fffffffffffffff) > 0x7ff0000000000000; }

    inline bool isInf() const { return (v & 0x7fffffffffffffff) == 0x7ff0000000000000; }

    inline bool isSubnormal() const { return ((v >> 52) & 0x7FF) == 0; }

    inline bool getSign() const { return (v >> 63) != 0; }

    softdouble setSign(bool sign) const
    {
        softdouble x;
        x.v = (v & ((1ULL << 63) - 1)) | ((uint_fast64_t)(sign) << 63);
        return x;
    }

    inline int getExp() const { return ((v >> 52) & 0x7FF) - 1023; }

    inline softdouble setExp(int e) const
    {
        softdouble x;
        x.v = (v & 0x800FFFFFFFFFFFFF) | ((uint_fast64_t)((e + 1023) & 0x7FF) << 52);
        return x;
    }

    inline softdouble getFrac() const
    {
        uint_fast64_t vv = (v & 0x000FFFFFFFFFFFFF) | ((uint_fast64_t)(1023) << 52);
        return softdouble::fromRaw(vv);
    }

    inline softdouble setFrac(const softdouble& s) const
    {
        softdouble x;
        x.v = (v & 0xFFF0000000000000) | (s.v & 0x000FFFFFFFFFFFFF);
        return x;
    }

    static softdouble zero() { return softdouble::fromRaw(0); }

    static softdouble inf() { return softdouble::fromRaw((uint_fast64_t)(0x7FF) << 52); }

    static softdouble nan() { return softdouble::fromRaw(HL_BIG_INT(0x7FFFFFFFFFFFFFFF)); }

    static softdouble one() { return softdouble::fromRaw((uint_fast64_t)(1023) << 52); }

    static softdouble min() { return softdouble::fromRaw((uint_fast64_t)(0x01) << 52); }

    static softdouble eps() { return softdouble::fromRaw((uint_fast64_t)(1023 - 52) << 52); }

    static softdouble max() { return softdouble::fromRaw(((uint_fast64_t)(0x7FF) << 52) - 1); }

    static softdouble pi() { return softdouble::fromRaw(HL_BIG_INT(0x400921FB54442D18)); }

    uint64_t v;
};

}    // namespace hl

/*----------------------------------------------------------------------------
| Ported from OpenHL and added for usability
*----------------------------------------------------------------------------*/

int hlTrunc(const hl::softfloat& a);
int hlTrunc(const hl::softdouble& a);

int hlRound(const hl::softfloat& a);
int hlRound(const hl::softdouble& a);

int64_t hlRound64(const hl::softdouble& a);

int hlFloor(const hl::softfloat& a);
int hlFloor(const hl::softdouble& a);

int hlCeil(const hl::softfloat& a);
int hlCeil(const hl::softdouble& a);

namespace hl
{

template <typename _Tp> inline static _Tp saturate_cast(softfloat a) { return _Tp(a); }

template <typename _Tp> inline static _Tp saturate_cast(softdouble a) { return _Tp(a); }

template <> inline uchar saturate_cast<uchar>(softfloat a) { return (uchar)std::max(std::min(hlRound(a), (int)UCHAR_MAX), 0); }

template <> inline uchar saturate_cast<uchar>(softdouble a) { return (uchar)std::max(std::min(hlRound(a), (int)UCHAR_MAX), 0); }

template <> inline schar saturate_cast<schar>(softfloat a) { return (schar)std::min(std::max(hlRound(a), (int)SCHAR_MIN), (int)SCHAR_MAX); }

template <> inline schar saturate_cast<schar>(softdouble a) { return (schar)std::min(std::max(hlRound(a), (int)SCHAR_MIN), (int)SCHAR_MAX); }

template <> inline ushort saturate_cast<ushort>(softfloat a) { return (ushort)std::max(std::min(hlRound(a), (int)USHRT_MAX), 0); }

template <> inline ushort saturate_cast<ushort>(softdouble a) { return (ushort)std::max(std::min(hlRound(a), (int)USHRT_MAX), 0); }

template <> inline short saturate_cast<short>(softfloat a) { return (short)std::min(std::max(hlRound(a), (int)SHRT_MIN), (int)SHRT_MAX); }

template <> inline short saturate_cast<short>(softdouble a) { return (short)std::min(std::max(hlRound(a), (int)SHRT_MIN), (int)SHRT_MAX); }

template <> inline int saturate_cast<int>(softfloat a) { return hlRound(a); }

template <> inline int saturate_cast<int>(softdouble a) { return hlRound(a); }

template <> inline int64_t saturate_cast<int64_t>(softfloat a) { return hlRound(a); }

template <> inline int64_t saturate_cast<int64_t>(softdouble a) { return hlRound64(a); }

template <> inline unsigned saturate_cast<unsigned>(softfloat a) { return hlRound(a); }

template <> inline unsigned saturate_cast<unsigned>(softdouble a) { return hlRound(a); }

template <> inline uint64_t saturate_cast<uint64_t>(softfloat a) { return hlRound(a); }

template <> inline uint64_t saturate_cast<uint64_t>(softdouble a) { return hlRound64(a); }

inline softfloat min(const softfloat& a, const softfloat& b) { return (a > b) ? b : a; }

inline softdouble min(const softdouble& a, const softdouble& b) { return (a > b) ? b : a; }

inline softfloat max(const softfloat& a, const softfloat& b) { return (a > b) ? a : b; }

inline softdouble max(const softdouble& a, const softdouble& b) { return (a > b) ? a : b; }

inline softfloat abs(softfloat a)
{
    softfloat x;
    x.v = a.v & ((1U << 31) - 1);
    return x;
}

inline softdouble abs(softdouble a)
{
    softdouble x;
    x.v = a.v & ((1ULL << 63) - 1);
    return x;
}

softfloat  exp(const softfloat& a);
softdouble exp(const softdouble& a);

softfloat  log(const softfloat& a);
softdouble log(const softdouble& a);

softfloat  pow(const softfloat& a, const softfloat& b);
softdouble pow(const softdouble& a, const softdouble& b);

softfloat cbrt(const softfloat& a);

softdouble sin(const softdouble& a);

softdouble cos(const softdouble& a);

}    // namespace hl