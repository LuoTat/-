#pragma once

#include <climits>
#include <cfloat>
#include <vector>
#include <limits>

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.hxx"
#include "openHL/core/matx.hxx"

namespace hl
{

//////////////////////////////// Size_ ////////////////////////////////

template <typename _Tp>
class Size_
{
public:
    typedef _Tp value_type;

    Size_();
    Size_(const Size_& sz)     = default;
    Size_(Size_&& sz) noexcept = default;
    Size_(_Tp _width, _Tp _height);

    Size_& operator=(const Size_& sz)     = default;
    Size_& operator=(Size_&& sz) noexcept = default;


    _Tp    area() const;
    double aspectRatio() const;
    bool   empty() const;
    template <typename _Tp2> operator Size_<_Tp2>() const;

    _Tp width;     //!< the width
    _Tp height;    //!< the height
};

typedef Size_<int>    Size2i;
typedef Size_<int64>  Size2l;
typedef Size_<float>  Size2f;
typedef Size_<double> Size2d;
typedef Size2i        Size;

template <typename _Tp> class DataType<Size_<_Tp>>
{
public:
    typedef Size_<_Tp>                               value_type;
    typedef Size_<typename DataType<_Tp>::work_type> work_type;
    typedef _Tp                                      channel_type;

    enum
    {
        generic_type = 0,
        channels     = 2,
        fmt          = DataType<channel_type>::fmt + ((channels - 1) << 8),
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Size_<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Size_<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 2)
    };
};
}    // namespace traits

//////////////////////////////// Scalar_ ///////////////////////////////

template <typename _Tp> class Scalar_: public Vec<_Tp, 4>
{
public:
    Scalar_();
    Scalar_(const Scalar_& s);
    Scalar_(Scalar_&& s) noexcept;
    Scalar_(_Tp v0);
    Scalar_(_Tp v0, _Tp v1, _Tp v2 = 0, _Tp v3 = 0);
    template <typename _Tp2, int cn> Scalar_(const Vec<_Tp2, cn>& v);

    Scalar_& operator=(const Scalar_& s);
    Scalar_& operator=(Scalar_&& s) noexcept;


    static Scalar_<_Tp> all(_Tp v0);

    template <typename T2> operator Scalar_<T2>() const;

    Scalar_<_Tp> mul(const Scalar_<_Tp>& a, double scale = 1) const;

    Scalar_<_Tp> conj() const;

    bool isReal() const;
};

typedef Scalar_<double> Scalar;

template <typename _Tp> class DataType<Scalar_<_Tp>>
{
public:
    typedef Scalar_<_Tp>                               value_type;
    typedef Scalar_<typename DataType<_Tp>::work_type> work_type;
    typedef _Tp                                        channel_type;

    enum
    {
        generic_type = 0,
        channels     = 4,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8),
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Scalar_<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Scalar_<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 4)
    };
};
}    // namespace traits

////////////////////////////////// Size /////////////////////////////////

template <typename _Tp>
inline Size_<_Tp>::Size_():
    width(0), height(0)
{}

template <typename _Tp>
inline Size_<_Tp>::Size_(_Tp _width, _Tp _height):
    width(_width), height(_height)
{}

template <typename _Tp> template <typename _Tp2>
inline Size_<_Tp>::operator Size_<_Tp2>() const
{
    return Size_<_Tp2>(saturate_cast<_Tp2>(width), saturate_cast<_Tp2>(height));
}

template <typename _Tp>
inline _Tp Size_<_Tp>::area() const
{
    const _Tp result = width * height;
    // make sure the result fits in the return value
    HL_DbgAssert(!std::numeric_limits<_Tp>::is_integer || width == 0 || result / width == height);
    return result;
}

template <typename _Tp>
inline double Size_<_Tp>::aspectRatio() const
{
    return width / static_cast<double>(height);
}

template <typename _Tp>
inline bool Size_<_Tp>::empty() const
{
    return width <= 0 || height <= 0;
}

template <typename _Tp>
inline static Size_<_Tp>& operator*=(Size_<_Tp>& a, _Tp b)
{
    a.width  *= b;
    a.height *= b;
    return a;
}

template <typename _Tp>
inline static Size_<_Tp> operator*(const Size_<_Tp>& a, _Tp b)
{
    Size_<_Tp> tmp(a);
    tmp *= b;
    return tmp;
}

template <typename _Tp>
inline static Size_<_Tp>& operator/=(Size_<_Tp>& a, _Tp b)
{
    a.width  /= b;
    a.height /= b;
    return a;
}

template <typename _Tp>
inline static Size_<_Tp> operator/(const Size_<_Tp>& a, _Tp b)
{
    Size_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _Tp>
inline static Size_<_Tp>& operator+=(Size_<_Tp>& a, const Size_<_Tp>& b)
{
    a.width  += b.width;
    a.height += b.height;
    return a;
}

template <typename _Tp>
inline static Size_<_Tp> operator+(const Size_<_Tp>& a, const Size_<_Tp>& b)
{
    Size_<_Tp> tmp(a);
    tmp += b;
    return tmp;
}

template <typename _Tp>
inline static Size_<_Tp>& operator-=(Size_<_Tp>& a, const Size_<_Tp>& b)
{
    a.width  -= b.width;
    a.height -= b.height;
    return a;
}

template <typename _Tp>
inline static Size_<_Tp> operator-(const Size_<_Tp>& a, const Size_<_Tp>& b)
{
    Size_<_Tp> tmp(a);
    tmp -= b;
    return tmp;
}

template <typename _Tp>
inline static bool operator==(const Size_<_Tp>& a, const Size_<_Tp>& b)
{
    return a.width == b.width && a.height == b.height;
}

template <typename _Tp>
inline static bool operator!=(const Size_<_Tp>& a, const Size_<_Tp>& b)
{
    return !(a == b);
}

///////////////////////////////// Scalar ////////////////////////////////

template <typename _Tp>
inline Scalar_<_Tp>::Scalar_()
{
    this->val[0] = this->val[1] = this->val[2] = this->val[3] = 0;
}

template <typename _Tp>
inline Scalar_<_Tp>::Scalar_(const Scalar_<_Tp>& s):
    Vec<_Tp, 4>(s)
{
}

template <typename _Tp>
inline Scalar_<_Tp>::Scalar_(Scalar_<_Tp>&& s) noexcept
{
    this->val[0] = std::move(s.val[0]);
    this->val[1] = std::move(s.val[1]);
    this->val[2] = std::move(s.val[2]);
    this->val[3] = std::move(s.val[3]);
}

template <typename _Tp>
inline Scalar_<_Tp>::Scalar_(_Tp v0)
{
    this->val[0] = v0;
    this->val[1] = this->val[2] = this->val[3] = 0;
}

template <typename _Tp>
inline Scalar_<_Tp>::Scalar_(_Tp v0, _Tp v1, _Tp v2, _Tp v3)
{
    this->val[0] = v0;
    this->val[1] = v1;
    this->val[2] = v2;
    this->val[3] = v3;
}

template <typename _Tp>
template <typename _Tp2, int cn>
inline Scalar_<_Tp>::Scalar_(const Vec<_Tp2, cn>& v)
{
    int i;
    for (i = 0; i < (cn < 4 ? cn : 4); i++)
        this->val[i] = hl::saturate_cast<_Tp>(v.val[i]);
    for (; i < 4; i++)
        this->val[i] = 0;
}

template <typename _Tp>
inline Scalar_<_Tp>& Scalar_<_Tp>::operator=(const Scalar_<_Tp>& s)
{
    this->val[0] = s.val[0];
    this->val[1] = s.val[1];
    this->val[2] = s.val[2];
    this->val[3] = s.val[3];
    return *this;
}

template <typename _Tp>
inline Scalar_<_Tp>& Scalar_<_Tp>::operator=(Scalar_<_Tp>&& s) noexcept
{
    this->val[0] = std::move(s.val[0]);
    this->val[1] = std::move(s.val[1]);
    this->val[2] = std::move(s.val[2]);
    this->val[3] = std::move(s.val[3]);
    return *this;
}

template <typename _Tp>
inline Scalar_<_Tp> Scalar_<_Tp>::all(_Tp v0)
{
    return Scalar_<_Tp>(v0, v0, v0, v0);
}

template <typename _Tp>
inline Scalar_<_Tp> Scalar_<_Tp>::mul(const Scalar_<_Tp>& a, double scale) const
{
    return Scalar_<_Tp>(saturate_cast<_Tp>(this->val[0] * a.val[0] * scale),
                        saturate_cast<_Tp>(this->val[1] * a.val[1] * scale),
                        saturate_cast<_Tp>(this->val[2] * a.val[2] * scale),
                        saturate_cast<_Tp>(this->val[3] * a.val[3] * scale));
}

template <typename _Tp>
inline Scalar_<_Tp> Scalar_<_Tp>::conj() const
{
    return Scalar_<_Tp>(saturate_cast<_Tp>(this->val[0]),
                        saturate_cast<_Tp>(-this->val[1]),
                        saturate_cast<_Tp>(-this->val[2]),
                        saturate_cast<_Tp>(-this->val[3]));
}

template <typename _Tp>
inline bool Scalar_<_Tp>::isReal() const
{
    return this->val[1] == 0 && this->val[2] == 0 && this->val[3] == 0;
}

template <typename _Tp>
template <typename T2> inline Scalar_<_Tp>::operator Scalar_<T2>() const
{
    return Scalar_<T2>(saturate_cast<T2>(this->val[0]),
                       saturate_cast<T2>(this->val[1]),
                       saturate_cast<T2>(this->val[2]),
                       saturate_cast<T2>(this->val[3]));
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator+=(Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    a.val[0] += b.val[0];
    a.val[1] += b.val[1];
    a.val[2] += b.val[2];
    a.val[3] += b.val[3];
    return a;
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator-=(Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    a.val[0] -= b.val[0];
    a.val[1] -= b.val[1];
    a.val[2] -= b.val[2];
    a.val[3] -= b.val[3];
    return a;
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator*=(Scalar_<_Tp>& a, _Tp v)
{
    a.val[0] *= v;
    a.val[1] *= v;
    a.val[2] *= v;
    a.val[3] *= v;
    return a;
}

template <typename _Tp>
inline static bool operator==(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return a.val[0] == b.val[0] && a.val[1] == b.val[1] && a.val[2] == b.val[2] && a.val[3] == b.val[3];
}

template <typename _Tp>
inline static bool operator!=(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return a.val[0] != b.val[0] || a.val[1] != b.val[1] || a.val[2] != b.val[2] || a.val[3] != b.val[3];
}

template <typename _Tp>
inline static Scalar_<_Tp> operator+(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return Scalar_<_Tp>(a.val[0] + b.val[0],
                        a.val[1] + b.val[1],
                        a.val[2] + b.val[2],
                        a.val[3] + b.val[3]);
}

template <typename _Tp>
inline static Scalar_<_Tp> operator-(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return Scalar_<_Tp>(saturate_cast<_Tp>(a.val[0] - b.val[0]),
                        saturate_cast<_Tp>(a.val[1] - b.val[1]),
                        saturate_cast<_Tp>(a.val[2] - b.val[2]),
                        saturate_cast<_Tp>(a.val[3] - b.val[3]));
}

template <typename _Tp>
inline static Scalar_<_Tp> operator*(const Scalar_<_Tp>& a, _Tp alpha)
{
    return Scalar_<_Tp>(a.val[0] * alpha,
                        a.val[1] * alpha,
                        a.val[2] * alpha,
                        a.val[3] * alpha);
}

template <typename _Tp>
inline static Scalar_<_Tp> operator*(_Tp alpha, const Scalar_<_Tp>& a)
{
    return a * alpha;
}

template <typename _Tp>
inline static Scalar_<_Tp> operator-(const Scalar_<_Tp>& a)
{
    return Scalar_<_Tp>(saturate_cast<_Tp>(-a.val[0]),
                        saturate_cast<_Tp>(-a.val[1]),
                        saturate_cast<_Tp>(-a.val[2]),
                        saturate_cast<_Tp>(-a.val[3]));
}

template <typename _Tp>
inline static Scalar_<_Tp> operator*(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return Scalar_<_Tp>(saturate_cast<_Tp>(a[0] * b[0] - a[1] * b[1] - a[2] * b[2] - a[3] * b[3]),
                        saturate_cast<_Tp>(a[0] * b[1] + a[1] * b[0] + a[2] * b[3] - a[3] * b[2]),
                        saturate_cast<_Tp>(a[0] * b[2] - a[1] * b[3] + a[2] * b[0] + a[3] * b[1]),
                        saturate_cast<_Tp>(a[0] * b[3] + a[1] * b[2] - a[2] * b[1] + a[3] * b[0]));
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator*=(Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    a = a * b;
    return a;
}

template <typename _Tp>
inline static Scalar_<_Tp> operator/(const Scalar_<_Tp>& a, _Tp alpha)
{
    return Scalar_<_Tp>(a.val[0] / alpha,
                        a.val[1] / alpha,
                        a.val[2] / alpha,
                        a.val[3] / alpha);
}

template <typename _Tp>
inline static Scalar_<float> operator/(const Scalar_<float>& a, float alpha)
{
    float s = 1 / alpha;
    return Scalar_<float>(a.val[0] * s, a.val[1] * s, a.val[2] * s, a.val[3] * s);
}

template <typename _Tp>
inline static Scalar_<double> operator/(const Scalar_<double>& a, double alpha)
{
    double s = 1 / alpha;
    return Scalar_<double>(a.val[0] * s, a.val[1] * s, a.val[2] * s, a.val[3] * s);
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator/=(Scalar_<_Tp>& a, _Tp alpha)
{
    a = a / alpha;
    return a;
}

template <typename _Tp>
inline static Scalar_<_Tp> operator/(_Tp a, const Scalar_<_Tp>& b)
{
    _Tp s = a / (b[0] * b[0] + b[1] * b[1] + b[2] * b[2] + b[3] * b[3]);
    return b.conj() * s;
}

template <typename _Tp>
inline static Scalar_<_Tp> operator/(const Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    return a * ((_Tp)1 / b);
}

template <typename _Tp>
inline static Scalar_<_Tp>& operator/=(Scalar_<_Tp>& a, const Scalar_<_Tp>& b)
{
    a = a / b;
    return a;
}

template <typename _Tp>
inline static Scalar operator*(const Matx<_Tp, 4, 4>& a, const Scalar& b)
{
    Matx<double, 4, 1> c((Matx<double, 4, 4>)a, b, Matx_MatMulOp());
    return reinterpret_cast<const Scalar&>(c);
}

template <> inline Scalar operator*(const Matx<double, 4, 4>& a, const Scalar& b)
{
    Matx<double, 4, 1> c(a, b, Matx_MatMulOp());
    return reinterpret_cast<const Scalar&>(c);
}

}    // namespace hl
