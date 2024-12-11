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

template <typename _Tp, int m, int n> class Matx
{
public:
    enum
    {
        rows     = m,
        cols     = n,
        channels = rows * cols,
        depth    = traits::Type<_Tp>::value,
        type     = HL_MAKETYPE(depth, channels),
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

template <typename _Tp, int m, int n> class DataType<Matx<_Tp, m, n>>
{
public:
    typedef Matx<_Tp, m, n>                               value_type;
    typedef Matx<typename DataType<_Tp>::work_type, m, n> work_type;
    typedef _Tp                                           channel_type;
    typedef value_type                                    vec_type;

    enum
    {
        generic_type = 0,
        channels     = m * n,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8),
        depth        = DataType<channel_type>::depth,
        type         = HL_MAKETYPE(depth, channels)

    };
};

namespace traits
{
template <typename _Tp, int m, int n>
struct Depth<Matx<_Tp, m, n>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp, int m, int n>
struct Type<Matx<_Tp, m, n>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, n* m)
    };
};
}    // namespace traits

/////////////////////// Vec (used as element of multi-channel images /////////////////////

template <typename _Tp, int cn> class Vec: public Matx<_Tp, cn, 1>
{
public:
    typedef _Tp value_type;

    enum
    {
        channels              = cn,
        depth                 = Matx<_Tp, cn, 1>::depth,
        type                  = HL_MAKETYPE(depth, channels),
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

template <typename _Tp, int cn> class DataType<Vec<_Tp, cn>>
{
public:
    typedef Vec<_Tp, cn>                               value_type;
    typedef Vec<typename DataType<_Tp>::work_type, cn> work_type;
    typedef _Tp                                        channel_type;
    typedef value_type                                 vec_type;

    enum
    {
        generic_type          = 0,
        channels              = cn,
        fmt                   = DataType<channel_type>::fmt + ((channels - 1) << 8),
        depth                 = DataType<channel_type>::depth,
        type                  = HL_MAKETYPE(depth, channels),
        _dummy_enum_finalizer = 0
    };
};

namespace traits
{
template <typename _Tp, int cn>
struct Depth<Vec<_Tp, cn>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp, int cn>
struct Type<Vec<_Tp, cn>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, cn)
    };
};
}    // namespace traits

///////////////////////////////////// helper classes /////////////////////////////////////
namespace internal
{

// TODO: Implement the LU function
template <typename _Tp, int m> struct Matx_DetOp
{
    double operator()(const Matx<_Tp, m, m>& a) const
    {
        Matx<_Tp, m, m> temp = a;
        double          p    = LU(temp.val, m * sizeof(_Tp), m, 0, 0, 0);
        if (p == 0)
            return p;
        for (int i = 0; i < m; i++)
            p *= temp(i, i);
        return p;
    }
};

template <typename _Tp> struct Matx_DetOp<_Tp, 1>
{
    double operator()(const Matx<_Tp, 1, 1>& a) const
    {
        return a(0, 0);
    }
};

template <typename _Tp> struct Matx_DetOp<_Tp, 2>
{
    double operator()(const Matx<_Tp, 2, 2>& a) const
    {
        return a(0, 0) * a(1, 1) - a(0, 1) * a(1, 0);
    }
};

template <typename _Tp> struct Matx_DetOp<_Tp, 3>
{
    double operator()(const Matx<_Tp, 3, 3>& a) const
    {
        return a(0, 0) * (a(1, 1) * a(2, 2) - a(2, 1) * a(1, 2)) - a(0, 1) * (a(1, 0) * a(2, 2) - a(2, 0) * a(1, 2)) + a(0, 2) * (a(1, 0) * a(2, 1) - a(2, 0) * a(1, 1));
    }
};

template <typename _Tp> inline Vec<_Tp, 2> conjugate(const Vec<_Tp, 2>& v)
{
    return Vec<_Tp, 2>(v[0], -v[1]);
}

template <typename _Tp> inline Vec<_Tp, 4> conjugate(const Vec<_Tp, 4>& v)
{
    return Vec<_Tp, 4>(v[0], -v[1], -v[2], -v[3]);
}

}    // namespace internal

////////////////////////////////// Matx Implementation ///////////////////////////////////

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx()
{
    for (int i = 0; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0)
{
    val[0] = v0;
    for (int i = 1; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1)
{
    HL_StaticAssert(channels >= 2, "Matx should have at least 2 elements.");
    val[0] = v0;
    val[1] = v1;
    for (int i = 2; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2)
{
    HL_StaticAssert(channels >= 3, "Matx should have at least 3 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    for (int i = 3; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3)
{
    HL_StaticAssert(channels >= 4, "Matx should have at least 4 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    for (int i = 4; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4)
{
    HL_StaticAssert(channels >= 5, "Matx should have at least 5 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    for (int i = 5; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5)
{
    HL_StaticAssert(channels >= 6, "Matx should have at least 6 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    val[5] = v5;
    for (int i = 6; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6)
{
    HL_StaticAssert(channels >= 7, "Matx should have at least 7 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    val[5] = v5;
    val[6] = v6;
    for (int i = 7; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7)
{
    HL_StaticAssert(channels >= 8, "Matx should have at least 8 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    val[5] = v5;
    val[6] = v6;
    val[7] = v7;
    for (int i = 8; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8)
{
    HL_StaticAssert(channels >= 9, "Matx should have at least 9 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    val[5] = v5;
    val[6] = v6;
    val[7] = v7;
    val[8] = v8;
    for (int i = 9; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9)
{
    HL_StaticAssert(channels >= 10, "Matx should have at least 10 elements.");
    val[0] = v0;
    val[1] = v1;
    val[2] = v2;
    val[3] = v3;
    val[4] = v4;
    val[5] = v5;
    val[6] = v6;
    val[7] = v7;
    val[8] = v8;
    val[9] = v9;
    for (int i = 10; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11)
{
    HL_StaticAssert(channels >= 12, "Matx should have at least 12 elements.");
    val[0]  = v0;
    val[1]  = v1;
    val[2]  = v2;
    val[3]  = v3;
    val[4]  = v4;
    val[5]  = v5;
    val[6]  = v6;
    val[7]  = v7;
    val[8]  = v8;
    val[9]  = v9;
    val[10] = v10;
    val[11] = v11;
    for (int i = 12; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13)
{
    HL_StaticAssert(channels >= 14, "Matx should have at least 14 elements.");
    val[0]  = v0;
    val[1]  = v1;
    val[2]  = v2;
    val[3]  = v3;
    val[4]  = v4;
    val[5]  = v5;
    val[6]  = v6;
    val[7]  = v7;
    val[8]  = v8;
    val[9]  = v9;
    val[10] = v10;
    val[11] = v11;
    val[12] = v12;
    val[13] = v13;
    for (int i = 14; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13, _Tp v14, _Tp v15)
{
    HL_StaticAssert(channels >= 16, "Matx should have at least 16 elements.");
    val[0]  = v0;
    val[1]  = v1;
    val[2]  = v2;
    val[3]  = v3;
    val[4]  = v4;
    val[5]  = v5;
    val[6]  = v6;
    val[7]  = v7;
    val[8]  = v8;
    val[9]  = v9;
    val[10] = v10;
    val[11] = v11;
    val[12] = v12;
    val[13] = v13;
    val[14] = v14;
    val[15] = v15;
    for (int i = 16; i < channels; i++) val[i] = _Tp(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const _Tp* values)
{
    for (int i = 0; i < channels; i++) val[i] = values[i];
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(std::initializer_list<_Tp> list)
{
    HL_DbgAssert(list.size() == channels);
    int i = 0;
    for (const auto& elem : list)
    {
        val[i++] = elem;
    }
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::all(_Tp alpha)
{
    Matx<_Tp, m, n> M;
    for (int i = 0; i < m * n; i++) M.val[i] = alpha;
    return M;
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::zeros()
{
    return all(0);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::ones()
{
    return all(1);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::eye()
{
    Matx<_Tp, m, n> M;
    for (int i = 0; i < shortdim; i++)
        M(i, i) = 1;
    return M;
}

template <typename _Tp, int m, int n>
inline _Tp Matx<_Tp, m, n>::dot(const Matx<_Tp, m, n>& M) const
{
    _Tp s = 0;
    for (int i = 0; i < channels; i++) s += val[i] * M.val[i];
    return s;
}

template <typename _Tp, int m, int n>
inline double Matx<_Tp, m, n>::ddot(const Matx<_Tp, m, n>& M) const
{
    double s = 0;
    for (int i = 0; i < channels; i++) s += (double)val[i] * M.val[i];
    return s;
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::diag(const typename Matx<_Tp, m, n>::diag_type& d)
{
    Matx<_Tp, m, n> M;
    for (int i = 0; i < shortdim; i++)
        M(i, i) = d(i, 0);
    return M;
}

template <typename _Tp, int m, int n>
template <typename T2>
inline Matx<_Tp, m, n>::operator Matx<T2, m, n>() const
{
    Matx<T2, m, n> M;
    for (int i = 0; i < m * n; i++) M.val[i] = saturate_cast<T2>(val[i]);
    return M;
}

template <typename _Tp, int m, int n>
template <int m1, int n1>
inline Matx<_Tp, m1, n1> Matx<_Tp, m, n>::reshape() const
{
    HL_StaticAssert(m1 * n1 == m * n, "Input and destination matrices must have the same number of elements");
    return (const Matx<_Tp, m1, n1>&)*this;
}

template <typename _Tp, int m, int n>
template <int m1, int n1>
inline Matx<_Tp, m1, n1> Matx<_Tp, m, n>::get_minor(int base_row, int base_col) const
{
    HL_DbgAssert(0 <= base_row && base_row + m1 <= m && 0 <= base_col && base_col + n1 <= n);
    Matx<_Tp, m1, n1> s;
    for (int di = 0; di < m1; di++)
        for (int dj = 0; dj < n1; dj++)
            s(di, dj) = (*this)(base_row + di, base_col + dj);
    return s;
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, 1, n> Matx<_Tp, m, n>::row(int i) const
{
    HL_DbgAssert((unsigned)i < (unsigned)m);
    return Matx<_Tp, 1, n>(&val[i * n]);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, 1> Matx<_Tp, m, n>::col(int j) const
{
    HL_DbgAssert((unsigned)j < (unsigned)n);
    Matx<_Tp, m, 1> v;
    for (int i = 0; i < m; i++)
        v.val[i] = val[i * n + j];
    return v;
}

template <typename _Tp, int m, int n> inline
    typename Matx<_Tp, m, n>::diag_type
    Matx<_Tp, m, n>::diag() const
{
    diag_type d;
    for (int i = 0; i < shortdim; i++)
        d.val[i] = val[i * n + i];
    return d;
}

template <typename _Tp, int m, int n>
const inline _Tp& Matx<_Tp, m, n>::operator()(int row_idx, int col_idx) const
{
    HL_DbgAssert((unsigned)row_idx < (unsigned)m && (unsigned)col_idx < (unsigned)n);
    return this->val[row_idx * n + col_idx];
}

template <typename _Tp, int m, int n>
inline _Tp& Matx<_Tp, m, n>::operator()(int row_idx, int col_idx)
{
    HL_DbgAssert((unsigned)row_idx < (unsigned)m && (unsigned)col_idx < (unsigned)n);
    return val[row_idx * n + col_idx];
}

template <typename _Tp, int m, int n>
const inline _Tp& Matx<_Tp, m, n>::operator()(int i) const
{
    HL_StaticAssert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
    HL_DbgAssert((unsigned)i < (unsigned)(m + n - 1));
    return val[i];
}

template <typename _Tp, int m, int n>
inline _Tp& Matx<_Tp, m, n>::operator()(int i)
{
    HL_StaticAssert(m == 1 || n == 1, "Single index indexation requires matrix to be a column or a row");
    HL_DbgAssert((unsigned)i < (unsigned)(m + n - 1));
    return val[i];
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_AddOp)
{
    for (int i = 0; i < channels; i++)
        val[i] = saturate_cast<_Tp>(a.val[i] + b.val[i]);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_SubOp)
{
    for (int i = 0; i < channels; i++)
        val[i] = saturate_cast<_Tp>(a.val[i] - b.val[i]);
}

template <typename _Tp, int m, int n>
template <typename _T2>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, n>& a, _T2 alpha, Matx_ScaleOp)
{
    for (int i = 0; i < channels; i++)
        val[i] = saturate_cast<_Tp>(a.val[i] * alpha);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_MulOp)
{
    for (int i = 0; i < channels; i++)
        val[i] = saturate_cast<_Tp>(a.val[i] * b.val[i]);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b, Matx_DivOp)
{
    for (int i = 0; i < channels; i++)
        val[i] = saturate_cast<_Tp>(a.val[i] / b.val[i]);
}

template <typename _Tp, int m, int n> template <int l>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, m, l>& a, const Matx<_Tp, l, n>& b, Matx_MatMulOp)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            _Tp s = 0;
            for (int k = 0; k < l; k++)
                s += a(i, k) * b(k, j);
            val[i * n + j] = s;
        }
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n>::Matx(const Matx<_Tp, n, m>& a, Matx_TOp)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            val[i * n + j] = a(j, i);
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::mul(const Matx<_Tp, m, n>& a) const
{
    return Matx<_Tp, m, n>(*this, a, Matx_MulOp());
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, m, n> Matx<_Tp, m, n>::div(const Matx<_Tp, m, n>& a) const
{
    return Matx<_Tp, m, n>(*this, a, Matx_DivOp());
}

template <typename _Tp, int m, int n>
inline Matx<_Tp, n, m> Matx<_Tp, m, n>::t() const
{
    return Matx<_Tp, n, m>(*this, Matx_TOp());
}

/////////////////////////////////// Vec Implementation ///////////////////////////////////

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec()
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0):
    Matx<_Tp, cn, 1>(v0)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1):
    Matx<_Tp, cn, 1>(v0, v1)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2):
    Matx<_Tp, cn, 1>(v0, v1, v2)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5, v6)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5, v6, v7)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5, v6, v7, v8)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(_Tp v0, _Tp v1, _Tp v2, _Tp v3, _Tp v4, _Tp v5, _Tp v6, _Tp v7, _Tp v8, _Tp v9, _Tp v10, _Tp v11, _Tp v12, _Tp v13):
    Matx<_Tp, cn, 1>(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(const _Tp* values):
    Matx<_Tp, cn, 1>(values)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(std::initializer_list<_Tp> list):
    Matx<_Tp, cn, 1>(list)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(const Vec<_Tp, cn>& m):
    Matx<_Tp, cn, 1>(m.val)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn> Vec<_Tp, cn>::all(_Tp alpha)
{
    Vec v;
    for (int i = 0; i < cn; i++) v.val[i] = alpha;
    return v;
}

template <typename _Tp, int cn>
inline Vec<_Tp, cn> Vec<_Tp, cn>::zeros()
{
    return Vec::all(0);
}

template <typename _Tp, int cn>
inline Vec<_Tp, cn> Vec<_Tp, cn>::ones()
{
    return Vec::all(1);
}

template <typename _Tp, int cn>
inline Vec<_Tp, cn> Vec<_Tp, cn>::mul(const Vec<_Tp, cn>& v) const
{
    Vec<_Tp, cn> w;
    for (int i = 0; i < cn; i++) w.val[i] = saturate_cast<_Tp>(this->val[i] * v.val[i]);
    return w;
}

template <> inline Vec<float, 2> Vec<float, 2>::conj() const
{
    return hl::internal::conjugate(*this);
}

template <> inline Vec<double, 2> Vec<double, 2>::conj() const
{
    return hl::internal::conjugate(*this);
}

template <> inline Vec<float, 4> Vec<float, 4>::conj() const
{
    return hl::internal::conjugate(*this);
}

template <> inline Vec<double, 4> Vec<double, 4>::conj() const
{
    return hl::internal::conjugate(*this);
}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(const Matx<_Tp, cn, 1>& a, const Matx<_Tp, cn, 1>& b, Matx_AddOp op):
    Matx<_Tp, cn, 1>(a, b, op)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn>::Vec(const Matx<_Tp, cn, 1>& a, const Matx<_Tp, cn, 1>& b, Matx_SubOp op):
    Matx<_Tp, cn, 1>(a, b, op)
{}

template <typename _Tp, int cn>
template <typename _T2>
inline Vec<_Tp, cn>::Vec(const Matx<_Tp, cn, 1>& a, _T2 alpha, Matx_ScaleOp op):
    Matx<_Tp, cn, 1>(a, alpha, op)
{}

template <typename _Tp, int cn>
inline Vec<_Tp, cn> Vec<_Tp, cn>::cross(const Vec<_Tp, cn>&) const
{
    HL_StaticAssert(cn == 3, "for arbitrary-size vector there is no cross-product defined");
    return Vec<_Tp, cn>();
}

template <> inline Vec<float, 3> Vec<float, 3>::cross(const Vec<float, 3>& v) const
{
    return Vec<float, 3>(this->val[1] * v.val[2] - this->val[2] * v.val[1],
                         this->val[2] * v.val[0] - this->val[0] * v.val[2],
                         this->val[0] * v.val[1] - this->val[1] * v.val[0]);
}

template <> inline Vec<double, 3> Vec<double, 3>::cross(const Vec<double, 3>& v) const
{
    return Vec<double, 3>(this->val[1] * v.val[2] - this->val[2] * v.val[1],
                          this->val[2] * v.val[0] - this->val[0] * v.val[2],
                          this->val[0] * v.val[1] - this->val[1] * v.val[0]);
}

template <typename _Tp, int cn>
template <typename T2>
inline Vec<_Tp, cn>::operator Vec<T2, cn>() const
{
    Vec<T2, cn> v;
    for (int i = 0; i < cn; i++) v.val[i] = saturate_cast<T2>(this->val[i]);
    return v;
}

template <typename _Tp, int cn>
const inline _Tp& Vec<_Tp, cn>::operator[](int i) const
{
    HL_DbgAssert((unsigned)i < (unsigned)cn);
    return this->val[i];
}

template <typename _Tp, int cn>
inline _Tp& Vec<_Tp, cn>::operator[](int i)
{
    HL_DbgAssert((unsigned)i < (unsigned)cn);
    return this->val[i];
}

template <typename _Tp, int cn>
const inline _Tp& Vec<_Tp, cn>::operator()(int i) const
{
    HL_DbgAssert((unsigned)i < (unsigned)cn);
    return this->val[i];
}

template <typename _Tp, int cn>
inline _Tp& Vec<_Tp, cn>::operator()(int i)
{
    HL_DbgAssert((unsigned)i < (unsigned)cn);
    return this->val[i];
}

///////////////////////////// Matx out-of-class operators ////////////////////////////////

template <typename _Tp1, typename _Tp2, int m, int n>
inline static Matx<_Tp1, m, n>& operator+=(Matx<_Tp1, m, n>& a, const Matx<_Tp2, m, n>& b)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = saturate_cast<_Tp1>(a.val[i] + b.val[i]);
    return a;
}

template <typename _Tp1, typename _Tp2, int m, int n>
inline static Matx<_Tp1, m, n>& operator-=(Matx<_Tp1, m, n>& a, const Matx<_Tp2, m, n>& b)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = saturate_cast<_Tp1>(a.val[i] - b.val[i]);
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator+(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b)
{
    return Matx<_Tp, m, n>(a, b, Matx_AddOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator-(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b)
{
    return Matx<_Tp, m, n>(a, b, Matx_SubOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, int alpha)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = saturate_cast<_Tp>(a.val[i] * alpha);
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, float alpha)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = saturate_cast<_Tp>(a.val[i] * alpha);
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator*=(Matx<_Tp, m, n>& a, double alpha)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = saturate_cast<_Tp>(a.val[i] * alpha);
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, int alpha)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, float alpha)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, n>& a, double alpha)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(int alpha, const Matx<_Tp, m, n>& a)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(float alpha, const Matx<_Tp, m, n>& a)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator*(double alpha, const Matx<_Tp, m, n>& a)
{
    return Matx<_Tp, m, n>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator/=(Matx<_Tp, m, n>& a, float alpha)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = a.val[i] / alpha;
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n>& operator/=(Matx<_Tp, m, n>& a, double alpha)
{
    for (int i = 0; i < m * n; i++)
        a.val[i] = a.val[i] / alpha;
    return a;
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator/(const Matx<_Tp, m, n>& a, float alpha)
{
    return Matx<_Tp, m, n>(a, 1.f / alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator/(const Matx<_Tp, m, n>& a, double alpha)
{
    return Matx<_Tp, m, n>(a, 1. / alpha, Matx_ScaleOp());
}

template <typename _Tp, int m, int n>
inline static Matx<_Tp, m, n> operator-(const Matx<_Tp, m, n>& a)
{
    return Matx<_Tp, m, n>(a, -1, Matx_ScaleOp());
}

template <typename _Tp, int m, int n, int l>
inline static Matx<_Tp, m, n> operator*(const Matx<_Tp, m, l>& a, const Matx<_Tp, l, n>& b)
{
    return Matx<_Tp, m, n>(a, b, Matx_MatMulOp());
}

template <typename _Tp, int m, int n>
inline static Vec<_Tp, m> operator*(const Matx<_Tp, m, n>& a, const Vec<_Tp, n>& b)
{
    Matx<_Tp, m, 1> c(a, b, Matx_MatMulOp());
    return (const Vec<_Tp, m>&)(c);
}

template <typename _Tp, int m, int n>
inline static bool operator==(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b)
{
    for (int i = 0; i < m * n; i++)
        if (a.val[i] != b.val[i]) return false;
    return true;
}

template <typename _Tp, int m, int n>
inline static bool operator!=(const Matx<_Tp, m, n>& a, const Matx<_Tp, m, n>& b)
{
    return !(a == b);
}

////////////////////////////// Vec out-of-class operators ////////////////////////////////

template <typename _Tp1, typename _Tp2, int cn>
inline static Vec<_Tp1, cn>& operator+=(Vec<_Tp1, cn>& a, const Vec<_Tp2, cn>& b)
{
    for (int i = 0; i < cn; i++)
        a.val[i] = saturate_cast<_Tp1>(a.val[i] + b.val[i]);
    return a;
}

template <typename _Tp1, typename _Tp2, int cn>
inline static Vec<_Tp1, cn>& operator-=(Vec<_Tp1, cn>& a, const Vec<_Tp2, cn>& b)
{
    for (int i = 0; i < cn; i++)
        a.val[i] = saturate_cast<_Tp1>(a.val[i] - b.val[i]);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator+(const Vec<_Tp, cn>& a, const Vec<_Tp, cn>& b)
{
    return Vec<_Tp, cn>(a, b, Matx_AddOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator-(const Vec<_Tp, cn>& a, const Vec<_Tp, cn>& b)
{
    return Vec<_Tp, cn>(a, b, Matx_SubOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, int alpha)
{
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * alpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, float alpha)
{
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * alpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator*=(Vec<_Tp, cn>& a, double alpha)
{
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * alpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, int alpha)
{
    double ialpha = 1. / alpha;
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * ialpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, float alpha)
{
    float ialpha = 1.f / alpha;
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * ialpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn>& operator/=(Vec<_Tp, cn>& a, double alpha)
{
    double ialpha = 1. / alpha;
    for (int i = 0; i < cn; i++)
        a[i] = saturate_cast<_Tp>(a[i] * ialpha);
    return a;
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, int alpha)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(int alpha, const Vec<_Tp, cn>& a)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, float alpha)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(float alpha, const Vec<_Tp, cn>& a)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(const Vec<_Tp, cn>& a, double alpha)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator*(double alpha, const Vec<_Tp, cn>& a)
{
    return Vec<_Tp, cn>(a, alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, int alpha)
{
    return Vec<_Tp, cn>(a, 1. / alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, float alpha)
{
    return Vec<_Tp, cn>(a, 1.f / alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator/(const Vec<_Tp, cn>& a, double alpha)
{
    return Vec<_Tp, cn>(a, 1. / alpha, Matx_ScaleOp());
}

template <typename _Tp, int cn>
inline static Vec<_Tp, cn> operator-(const Vec<_Tp, cn>& a)
{
    Vec<_Tp, cn> t;
    for (int i = 0; i < cn; i++) t.val[i] = saturate_cast<_Tp>(-a.val[i]);
    return t;
}

template <typename _Tp>
inline Vec<_Tp, 4> operator*(const Vec<_Tp, 4>& v1, const Vec<_Tp, 4>& v2)
{
    return Vec<_Tp, 4>(saturate_cast<_Tp>(v1[0] * v2[0] - v1[1] * v2[1] - v1[2] * v2[2] - v1[3] * v2[3]),
                       saturate_cast<_Tp>(v1[0] * v2[1] + v1[1] * v2[0] + v1[2] * v2[3] - v1[3] * v2[2]),
                       saturate_cast<_Tp>(v1[0] * v2[2] - v1[1] * v2[3] + v1[2] * v2[0] + v1[3] * v2[1]),
                       saturate_cast<_Tp>(v1[0] * v2[3] + v1[1] * v2[2] - v1[2] * v2[1] + v1[3] * v2[0]));
}

template <typename _Tp>
inline Vec<_Tp, 4>& operator*=(Vec<_Tp, 4>& v1, const Vec<_Tp, 4>& v2)
{
    v1 = v1 * v2;
    return v1;
}

}    // namespace hl