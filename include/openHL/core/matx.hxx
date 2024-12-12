#pragma once


#include "openHL/core/hldef.h"
#include "openHL/core/base.hxx"
#include "openHL/core/traits.hxx"
#include "openHL/core/saturate.hxx"

#include <initializer_list>

namespace hl
{

////////////////////////////// Small Matrix ///////////////////////////

struct Matx_AddOp
{
    Matx_AddOp() {}

    Matx_AddOp(const Matx_AddOp&) {}
};

struct Matx_SubOp
{
    Matx_SubOp() {}

    Matx_SubOp(const Matx_SubOp&) {}
};

struct Matx_ScaleOp
{
    Matx_ScaleOp() {}

    Matx_ScaleOp(const Matx_ScaleOp&) {}
};

struct Matx_MulOp
{
    Matx_MulOp() {}

    Matx_MulOp(const Matx_MulOp&) {}
};

struct Matx_DivOp
{
    Matx_DivOp() {}

    Matx_DivOp(const Matx_DivOp&) {}
};

struct Matx_MatMulOp
{
    Matx_MatMulOp() {}

    Matx_MatMulOp(const Matx_MatMulOp&) {}
};

struct Matx_TOp
{
    Matx_TOp() {}

    Matx_TOp(const Matx_TOp&) {}
};

template <typename _Tp, int m, int n>
class Matx
{
public:
    enum
    {
        rows     = m,
        cols     = n,
        channels = rows * cols,
        shortdim = (m < n ? m : n)
    };

    typedef _Tp                    value_type;
    typedef Matx<_Tp, m, n>        mat_type;
    typedef Matx<_Tp, shortdim, 1> diag_type;

    Matx();
    explicit Matx(_Tp v0);
    Matx(_Tp v0, _Tp v1);
    Matx(_Tp v0, _Tp v1, _Tp v2);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13);
    Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13, _Tp v14, _Tp v15);
    explicit Matx(const _Tp* vals);
    Matx(std::initializer_list<_Tp>);

    [[nodiscard]]
    static Matx all(_Tp alpha);
    [[nodiscard]]
    static Matx zeros();
    [[nodiscard]]
    static Matx ones();
    [[nodiscard]]
    static Matx eye();
    [[nodiscard]]
    static Matx diag(const diag_type& d);


    _Tp    dot(const Matx<_Tp, m, n>& v) const;
    double ddot(const Matx<_Tp, m, n>& v) const;
    template <typename T2> operator Matx<T2, m, n>() const;
    template <int m1, int n1> Matx<_Tp, m1, n1> reshape() const;
    template <int m1, int n1> Matx<_Tp, m1, n1> get_minor(int base_row, int base_col) const;
    Matx<_Tp, 1, n>                             row(int i) const;
    Matx<_Tp, m, 1>                             col(int i) const;
    diag_type                                   diag() const;
    Matx<_Tp, n, m>                             t() const;
    Matx<_Tp, m, n>                             mul(const Matx<_Tp, m, n>& a) const;
    Matx<_Tp, m, n>                             div(const Matx<_Tp, m, n>& a) const;


    const _Tp& operator()(int row, int col) const;
    _Tp&       operator()(int row, int col);
    const _Tp& operator()(int i) const;
    _Tp&       operator()(int i);

    Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_AddOp);
    Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_SubOp);
    template <typename _T2> Matx(const Matx<_Tp, m, n>& a, _T2 alpha, Matx_ScaleOp);
    Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_MulOp);
    Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_DivOp);
    template <int l> Matx(const Matx<_Tp, m, l>& a, const Matx<_Tp, l, n>& b, Matx_MatMulOp);
    Matx(const Matx<_Tp, n, m>& a, Matx_TOp);

    _Tp val[m * n];
};

typedef Matx<float, 1, 2>  Matx12f;
typedef Matx<double, 1, 2> Matx12d;
typedef Matx<float, 1, 3>  Matx13f;
typedef Matx<double, 1, 3> Matx13d;
typedef Matx<float, 1, 4>  Matx14f;
typedef Matx<double, 1, 4> Matx14d;
typedef Matx<float, 1, 6>  Matx16f;
typedef Matx<double, 1, 6> Matx16d;

typedef Matx<float, 2, 1>  Matx21f;
typedef Matx<double, 2, 1> Matx21d;
typedef Matx<float, 3, 1>  Matx31f;
typedef Matx<double, 3, 1> Matx31d;
typedef Matx<float, 4, 1>  Matx41f;
typedef Matx<double, 4, 1> Matx41d;
typedef Matx<float, 6, 1>  Matx61f;
typedef Matx<double, 6, 1> Matx61d;

typedef Matx<float, 2, 2>  Matx22f;
typedef Matx<double, 2, 2> Matx22d;
typedef Matx<float, 2, 3>  Matx23f;
typedef Matx<double, 2, 3> Matx23d;
typedef Matx<float, 3, 2>  Matx32f;
typedef Matx<double, 3, 2> Matx32d;

typedef Matx<float, 3, 3>  Matx33f;
typedef Matx<double, 3, 3> Matx33d;

typedef Matx<float, 3, 4>  Matx34f;
typedef Matx<double, 3, 4> Matx34d;
typedef Matx<float, 4, 3>  Matx43f;
typedef Matx<double, 4, 3> Matx43d;

typedef Matx<float, 4, 4>  Matx44f;
typedef Matx<double, 4, 4> Matx44d;
typedef Matx<float, 6, 6>  Matx66f;
typedef Matx<double, 6, 6> Matx66d;

template <typename _Tp, int m>
inline static double determinant(const Matx<_Tp, m, m>& a);

template <typename _Tp, int m, int n>
inline static double trace(const Matx<_Tp, m, n>& a);

template <typename _Tp, int m, int n>
inline static double norm(const Matx<_Tp, m, n>& M);

template <typename _Tp, int m, int n>
inline static double norm(const Matx<_Tp, m, n>& M, int normType);

template <typename _Tp1, typename _Tp2, int m, int n>
inline static Matx<_Tp1, m, n>& operator+=(Matx<_Tp1, m, n>& a, const Matx<_Tp2, m, n>& b);

template <typename _Tp1, typename _Tp2, int m, int n>
inline static Matx<_Tp1, m, n>& operator-=(Matx<_Tp1, m, n>& a, const Matx<_Tp2, m, n>& b);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator+(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator-(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, int alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, float alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, double alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, int alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, float alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, double alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(int alpha, const Matx<_Tp, m, n>& a);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(float alpha, const Matx<_Tp, m, n>& a);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(double alpha, const Matx<_Tp, m, n>& a);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator/=(Matx<_Tp, m, n>& a, float alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator/=(Matx<_Tp, m, n>& a, double alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator/(const Matx<_Tp, m, n>& a, float alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator/(const Matx<_Tp, m, n>& a, double alpha);

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator-(const Matx<_Tp, m, n>& a);

template <typename _Tp, int m, int n, int l>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, l>& a, const Matx<_Tp, l, n>& b);

template <typename _Tp, int m, int n>
inline static Vec<_Tp, m> operator*(const Matx<_Tp, m, n>& a, const Vec<_Tp, n>& b);

template <typename _Tp, int m, int n>
inline static bool operator==(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b);

template <typename _Tp, int m, int n>
inline static bool operator!=(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b);

/////////////////////// Vec (used as element of multi-channel images /////////////////////

template <typename _Tp, int cn>
class Vec: public Matx<_Tp, cn, 1>
{
public:
    typedef _Tp value_type;

    enum
    {
        channels              = cn,
        _dummy_enum_finalizer = 0
    };

    Vec();
    Vec(const Vec<_Tp, cn>& v);
    Vec(_Tp v0);
    Vec(_Tp v0, _Tp v1);
    Vec(_Tp v0, _Tp v1, _Tp v2);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9);
    Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13);
    explicit Vec(const _Tp* values);
    Vec(std::initializer_list<_Tp>);


    static Vec all(_Tp alpha);
    static Vec zeros();
    static Vec ones();
    static Vec eye()           = delete;
    static Vec diag(_Tp alpha) = delete;


    Vec mul(const Vec<_Tp, cn>& v) const;
    Vec conj() const;
    Vec cross(const Vec& v) const;
    template <typename T2> operator Vec<T2, cn>() const;
    const _Tp&    operator[](int i) const;
    _Tp&          operator[](int i);
    const _Tp&    operator()(int i) const;
    _Tp&          operator()(int i);
    Vec<_Tp, cn>& operator=(const Vec<_Tp, cn>& rhs) = default;


    Vec(const Matx<_Tp, cn, 1>& a, const Matx<_Tp, cn, 1>& b, Matx_AddOp);
    Vec(const Matx<_Tp, cn, 1>& a, const Matx<_Tp, cn, 1>& b, Matx_SubOp);
    template <typename _T2> Vec(const Matx<_Tp, cn, 1>& a, _T2 alpha, Matx_ScaleOp);
};

typedef Vec<uchar, 2> Vec2b;
typedef Vec<uchar, 3> Vec3b;
typedef Vec<uchar, 4> Vec4b;

typedef Vec<short, 2> Vec2s;
typedef Vec<short, 3> Vec3s;
typedef Vec<short, 4> Vec4s;

typedef Vec<ushort, 2> Vec2w;
typedef Vec<ushort, 3> Vec3w;
typedef Vec<ushort, 4> Vec4w;

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;
typedef Vec<int, 4> Vec4i;
typedef Vec<int, 6> Vec6i;
typedef Vec<int, 8> Vec8i;

typedef Vec<float, 2> Vec2f;
typedef Vec<float, 3> Vec3f;
typedef Vec<float, 4> Vec4f;
typedef Vec<float, 6> Vec6f;

typedef Vec<double, 2> Vec2d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 4> Vec4d;
typedef Vec<double, 6> Vec6d;

template <typename _Tp, int cn>
inline Vec<_Tp, cn> normalize(const Vec<_Tp, cn>& v);

template <typename _Tp1, typename _Tp2, int cn>
inline static Vec<_Tp1, cn>& operator+=(Vec<_Tp1, cn>& a, const Vec<_Tp2, cn>& b);

template <typename _Tp1, typename _Tp2, int cn>
inline static Vec<_Tp1, cn>& operator-=(Vec<_Tp1, cn>& a, const Vec<_Tp2, cn>& b);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator+(const Vec<_Tp, cn>& a, const Vec<_Tp, cn>& b);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator-(const Vec<_Tp, cn>& a, const Vec<_Tp, cn>& b);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, int alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, float alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, double alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, int alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, float alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, double alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, int alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(int alpha, const Vec<_Tp, cn>& a);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, float alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(float alpha, const Vec<_Tp, cn>& a);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, double alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(double alpha, const Vec<_Tp, cn>& a);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, int alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, float alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, double alpha);

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator-(const Vec<_Tp, cn>& a);

template <typename _Tp>
inline Vec<_Tp, 4> operator*(const Vec<_Tp, 4>& v1, const Vec<_Tp, 4>& v2);

template <typename _Tp>
inline Vec<_Tp, 4>& operator*=(Vec<_Tp, 4>& v1, const Vec<_Tp, 4>& v2);

}    // namespace hl

#include "openHL/core/matx.inl.hxx"
