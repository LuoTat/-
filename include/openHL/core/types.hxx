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

//////////////////////////////// Complex //////////////////////////////

template <typename _Tp> class Complex
{
public:
    //! default constructor
    Complex();
    Complex(_Tp _re, _Tp _im = 0);

    //! conversion to another data type
    template <typename T2> operator Complex<T2>() const;
    //! conjugation
    Complex conj() const;

    _Tp re, im;    ///< the real and the imaginary parts
};

typedef Complex<float>  Complexf;
typedef Complex<double> Complexd;

template <typename _Tp> class DataType<Complex<_Tp>>
{
public:
    typedef Complex<_Tp> value_type;
    typedef value_type   work_type;
    typedef _Tp          channel_type;

    enum
    {
        generic_type = 0,
        channels     = 2,
        fmt          = DataType<channel_type>::fmt + ((channels - 1) << 8)
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Complex<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Complex<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 2)
    };
};
}    // namespace traits

//////////////////////////////// Point_ ////////////////////////////////

template <typename _Tp> class Point_
{
public:
    typedef _Tp value_type;

    //! default constructor
    Point_();
    Point_(_Tp _x, _Tp _y);
    Point_(const Point_& pt)     = default;
    Point_(Point_&& pt) noexcept = default;
    Point_(const Size_<_Tp>& sz);
    Point_(const Vec<_Tp, 2>& v);

    Point_& operator=(const Point_& pt)     = default;
    Point_& operator=(Point_&& pt) noexcept = default;
    //! conversion to another data type
    template <typename _Tp2> operator Point_<_Tp2>() const;

    //! conversion to the old-style C structures
    operator Vec<_Tp, 2>() const;

    //! dot product
    _Tp dot(const Point_& pt) const;
    //! dot product computed in double-precision arithmetics
    double ddot(const Point_& pt) const;
    //! cross-product
    double cross(const Point_& pt) const;
    //! checks whether the point is inside the specified rectangle
    bool inside(const Rect_<_Tp>& r) const;
    _Tp  x;    //!< x coordinate of the point
    _Tp  y;    //!< y coordinate of the point
};

typedef Point_<int>    Point2i;
typedef Point_<int64>  Point2l;
typedef Point_<float>  Point2f;
typedef Point_<double> Point2d;
typedef Point2i        Point;

template <typename _Tp> class DataType<Point_<_Tp>>
{
public:
    typedef Point_<_Tp>                               value_type;
    typedef Point_<typename DataType<_Tp>::work_type> work_type;
    typedef _Tp                                       channel_type;

    enum
    {
        generic_type = 0,
        channels     = 2,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8)
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Point_<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Point_<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 2)
    };
};
}    // namespace traits

//////////////////////////////// Point3_ ////////////////////////////////

template <typename _Tp> class Point3_
{
public:
    typedef _Tp value_type;

    //! default constructor
    Point3_();
    Point3_(_Tp _x, _Tp _y, _Tp _z);
    Point3_(const Point3_& pt)     = default;
    Point3_(Point3_&& pt) noexcept = default;
    explicit Point3_(const Point_<_Tp>& pt);
    Point3_(const Vec<_Tp, 3>& v);

    Point3_& operator=(const Point3_& pt)     = default;
    Point3_& operator=(Point3_&& pt) noexcept = default;
    //! conversion to another data type
    template <typename _Tp2> operator Point3_<_Tp2>() const;
    //! conversion to cv::Vec<>
    operator Vec<_Tp, 3>() const;

    //! dot product
    _Tp dot(const Point3_& pt) const;
    //! dot product computed in double-precision arithmetics
    double ddot(const Point3_& pt) const;
    //! cross product of the 2 3D points
    Point3_ cross(const Point3_& pt) const;
    _Tp     x;    //!< x coordinate of the 3D point
    _Tp     y;    //!< y coordinate of the 3D point
    _Tp     z;    //!< z coordinate of the 3D point
};

typedef Point3_<int>    Point3i;
typedef Point3_<float>  Point3f;
typedef Point3_<double> Point3d;

template <typename _Tp> class DataType<Point3_<_Tp>>
{
public:
    typedef Point3_<_Tp>                               value_type;
    typedef Point3_<typename DataType<_Tp>::work_type> work_type;
    typedef _Tp                                        channel_type;

    enum
    {
        generic_type = 0,
        channels     = 3,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8)
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Point3_<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Point3_<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 3)
    };
};
}    // namespace traits

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

//////////////////////////////// Rect_ ////////////////////////////////

template <typename _Tp> class Rect_
{
public:
    typedef _Tp value_type;

    //! default constructor
    Rect_();
    Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height);
    Rect_(const Rect_& r)     = default;
    Rect_(Rect_&& r) noexcept = default;
    Rect_(const Point_<_Tp>& org, const Size_<_Tp>& sz);
    Rect_(const Point_<_Tp>& pt1, const Point_<_Tp>& pt2);
    Rect_& operator=(const Rect_& r)     = default;
    Rect_& operator=(Rect_&& r) noexcept = default;

    //! the top-left corner
    Point_<_Tp> tl() const;
    //! the bottom-right corner
    Point_<_Tp> br() const;

    //! size (width, height) of the rectangle
    Size_<_Tp> size() const;
    //! area (width*height) of the rectangle
    _Tp area() const;
    //! true if empty
    bool empty() const;

    //! conversion to another data type
    template <typename _Tp2> operator Rect_<_Tp2>() const;

    //! checks whether the rectangle contains the point
    bool contains(const Point_<_Tp>& pt) const;

    _Tp x;         //!< x coordinate of the top-left corner
    _Tp y;         //!< y coordinate of the top-left corner
    _Tp width;     //!< width of the rectangle
    _Tp height;    //!< height of the rectangle
};

typedef Rect_<int>    Rect2i;
typedef Rect_<float>  Rect2f;
typedef Rect_<double> Rect2d;
typedef Rect2i        Rect;

template <typename _Tp> class DataType<Rect_<_Tp>>
{
public:
    typedef Rect_<_Tp>                               value_type;
    typedef Rect_<typename DataType<_Tp>::work_type> work_type;
    typedef _Tp                                      channel_type;

    enum
    {
        generic_type = 0,
        channels     = 4,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8)
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <typename _Tp>
struct Depth<Rect_<_Tp>>
{
    enum
    {
        value = Depth<_Tp>::value
    };
};

template <typename _Tp>
struct Type<Rect_<_Tp>>
{
    enum
    {
        value = HL_MAKETYPE(Depth<_Tp>::value, 4)
    };
};
}    // namespace traits

//////////////////////////////// Range /////////////////////////////////

class Range
{
public:
    Range();
    Range(int _start, int _end);
    int          size() const;
    bool         empty() const;
    static Range all();

    int start, end;
};

template <> class DataType<Range>
{
public:
    typedef Range      value_type;
    typedef value_type work_type;
    typedef int        channel_type;

    enum
    {
        generic_type = 0,
        channels     = 2,
        fmt          = traits::SafeFmt<channel_type>::fmt + ((channels - 1) << 8)
    };

    typedef Vec<channel_type, channels> vec_type;
};

namespace traits
{
template <>
struct Depth<Range>
{
    enum
    {
        value = Depth<int>::value
    };
};

template <>
struct Type<Range>
{
    enum
    {
        value = HL_MAKETYPE(Depth<int>::value, 2)
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

/////////////////////////////////////////////////////////////////////////
///////////////////////////// Implementation ////////////////////////////
/////////////////////////////////////////////////////////////////////////

//////////////////////////////// Complex ////////////////////////////////

template <typename _Tp>
inline Complex<_Tp>::Complex():
    re(0), im(0)
{}

template <typename _Tp>
inline Complex<_Tp>::Complex(_Tp _re, _Tp _im):
    re(_re), im(_im)
{}

template <typename _Tp>
template <typename T2>
inline Complex<_Tp>::operator Complex<T2>() const
{
    return Complex<T2>(saturate_cast<T2>(re), saturate_cast<T2>(im));
}

template <typename _Tp>
inline Complex<_Tp> Complex<_Tp>::conj() const
{
    return Complex<_Tp>(re, -im);
}

template <typename _Tp>
inline static bool operator==(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    return a.re == b.re && a.im == b.im;
}

template <typename _Tp>
inline static bool operator!=(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    return a.re != b.re || a.im != b.im;
}

template <typename _Tp>
inline static Complex<_Tp> operator+(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    return Complex<_Tp>(a.re + b.re, a.im + b.im);
}

template <typename _Tp>
inline static Complex<_Tp>& operator+=(Complex<_Tp>& a, const Complex<_Tp>& b)
{
    a.re += b.re;
    a.im += b.im;
    return a;
}

template <typename _Tp>
inline static Complex<_Tp> operator-(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    return Complex<_Tp>(a.re - b.re, a.im - b.im);
}

template <typename _Tp>
inline static Complex<_Tp>& operator-=(Complex<_Tp>& a, const Complex<_Tp>& b)
{
    a.re -= b.re;
    a.im -= b.im;
    return a;
}

template <typename _Tp>
inline static Complex<_Tp> operator-(const Complex<_Tp>& a)
{
    return Complex<_Tp>(-a.re, -a.im);
}

template <typename _Tp>
inline static Complex<_Tp> operator*(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    return Complex<_Tp>(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re);
}

template <typename _Tp>
inline static Complex<_Tp> operator*(const Complex<_Tp>& a, _Tp b)
{
    return Complex<_Tp>(a.re * b, a.im * b);
}

template <typename _Tp>
inline static Complex<_Tp> operator*(_Tp b, const Complex<_Tp>& a)
{
    return Complex<_Tp>(a.re * b, a.im * b);
}

template <typename _Tp>
inline static Complex<_Tp> operator+(const Complex<_Tp>& a, _Tp b)
{
    return Complex<_Tp>(a.re + b, a.im);
}

template <typename _Tp>
inline static Complex<_Tp> operator-(const Complex<_Tp>& a, _Tp b)
{
    return Complex<_Tp>(a.re - b, a.im);
}

template <typename _Tp>
inline static Complex<_Tp> operator+(_Tp b, const Complex<_Tp>& a)
{
    return Complex<_Tp>(a.re + b, a.im);
}

template <typename _Tp>
inline static Complex<_Tp> operator-(_Tp b, const Complex<_Tp>& a)
{
    return Complex<_Tp>(b - a.re, -a.im);
}

template <typename _Tp>
inline static Complex<_Tp>& operator+=(Complex<_Tp>& a, _Tp b)
{
    a.re += b;
    return a;
}

template <typename _Tp>
inline static Complex<_Tp>& operator-=(Complex<_Tp>& a, _Tp b)
{
    a.re -= b;
    return a;
}

template <typename _Tp>
inline static Complex<_Tp>& operator*=(Complex<_Tp>& a, _Tp b)
{
    a.re *= b;
    a.im *= b;
    return a;
}

template <typename _Tp>
inline static double abs(const Complex<_Tp>& a)
{
    return std::sqrt((double)a.re * a.re + (double)a.im * a.im);
}

template <typename _Tp>
inline static Complex<_Tp> operator/(const Complex<_Tp>& a, const Complex<_Tp>& b)
{
    double t = 1. / ((double)b.re * b.re + (double)b.im * b.im);
    return Complex<_Tp>((_Tp)((a.re * b.re + a.im * b.im) * t),
                        (_Tp)((-a.re * b.im + a.im * b.re) * t));
}

template <typename _Tp>
inline static Complex<_Tp>& operator/=(Complex<_Tp>& a, const Complex<_Tp>& b)
{
    a = a / b;
    return a;
}

template <typename _Tp>
inline static Complex<_Tp> operator/(const Complex<_Tp>& a, _Tp b)
{
    _Tp t = (_Tp)1 / b;
    return Complex<_Tp>(a.re * t, a.im * t);
}

template <typename _Tp>
inline static Complex<_Tp> operator/(_Tp b, const Complex<_Tp>& a)
{
    return Complex<_Tp>(b) / a;
}

template <typename _Tp>
inline static Complex<_Tp> operator/=(const Complex<_Tp>& a, _Tp b)
{
    _Tp t  = (_Tp)1 / b;
    a.re  *= t;
    a.im  *= t;
    return a;
}

//////////////////////////////// 2D Point ///////////////////////////////

template <typename _Tp>
inline Point_<_Tp>::Point_():
    x(0), y(0)
{}

template <typename _Tp>
inline Point_<_Tp>::Point_(_Tp _x, _Tp _y):
    x(_x), y(_y)
{}

template <typename _Tp>
inline Point_<_Tp>::Point_(const Size_<_Tp>& sz):
    x(sz.width), y(sz.height)
{}

template <typename _Tp>
inline Point_<_Tp>::Point_(const Vec<_Tp, 2>& v):
    x(v[0]), y(v[1])
{}

template <typename _Tp>
template <typename _Tp2>
inline Point_<_Tp>::operator Point_<_Tp2>() const
{
    return Point_<_Tp2>(saturate_cast<_Tp2>(x), saturate_cast<_Tp2>(y));
}

template <typename _Tp>
inline Point_<_Tp>::operator Vec<_Tp, 2>() const
{
    return Vec<_Tp, 2>(x, y);
}

template <typename _Tp>
inline _Tp Point_<_Tp>::dot(const Point_& pt) const
{
    return saturate_cast<_Tp>(x * pt.x + y * pt.y);
}

template <typename _Tp>
inline double Point_<_Tp>::ddot(const Point_& pt) const
{
    return (double)x * (double)(pt.x) + (double)y * (double)(pt.y);
}

template <typename _Tp>
inline double Point_<_Tp>::cross(const Point_& pt) const
{
    return (double)x * pt.y - (double)y * pt.x;
}

template <typename _Tp>
inline bool Point_<_Tp>::inside(const Rect_<_Tp>& r) const
{
    return r.contains(*this);
}

template <typename _Tp>
inline static Point_<_Tp>& operator+=(Point_<_Tp>& a, const Point_<_Tp>& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator-=(Point_<_Tp>& a, const Point_<_Tp>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator*=(Point_<_Tp>& a, int b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator*=(Point_<_Tp>& a, float b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator*=(Point_<_Tp>& a, double b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator/=(Point_<_Tp>& a, int b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator/=(Point_<_Tp>& a, float b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    return a;
}

template <typename _Tp>
inline static Point_<_Tp>& operator/=(Point_<_Tp>& a, double b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    return a;
}

template <typename _Tp>
inline static double norm(const Point_<_Tp>& pt)
{
    return std::sqrt((double)pt.x * pt.x + (double)pt.y * pt.y);
}

template <typename _Tp>
inline static bool operator==(const Point_<_Tp>& a, const Point_<_Tp>& b)
{
    return a.x == b.x && a.y == b.y;
}

template <typename _Tp>
inline static bool operator!=(const Point_<_Tp>& a, const Point_<_Tp>& b)
{
    return a.x != b.x || a.y != b.y;
}

template <typename _Tp>
inline static Point_<_Tp> operator+(const Point_<_Tp>& a, const Point_<_Tp>& b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(a.x + b.x), saturate_cast<_Tp>(a.y + b.y));
}

template <typename _Tp>
inline static Point_<_Tp> operator-(const Point_<_Tp>& a, const Point_<_Tp>& b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(a.x - b.x), saturate_cast<_Tp>(a.y - b.y));
}

template <typename _Tp>
inline static Point_<_Tp> operator-(const Point_<_Tp>& a)
{
    return Point_<_Tp>(saturate_cast<_Tp>(-a.x), saturate_cast<_Tp>(-a.y));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(const Point_<_Tp>& a, int b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(int a, const Point_<_Tp>& b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(const Point_<_Tp>& a, float b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(float a, const Point_<_Tp>& b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(const Point_<_Tp>& a, double b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(double a, const Point_<_Tp>& b)
{
    return Point_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a));
}

template <typename _Tp>
inline static Point_<_Tp> operator*(const Matx<_Tp, 2, 2>& a, const Point_<_Tp>& b)
{
    Matx<_Tp, 2, 1> tmp = a * Vec<_Tp, 2>(b.x, b.y);
    return Point_<_Tp>(tmp.val[0], tmp.val[1]);
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(const Matx<_Tp, 3, 3>& a, const Point_<_Tp>& b)
{
    Matx<_Tp, 3, 1> tmp = a * Vec<_Tp, 3>(b.x, b.y, 1);
    return Point3_<_Tp>(tmp.val[0], tmp.val[1], tmp.val[2]);
}

template <typename _Tp>
inline static Point_<_Tp> operator/(const Point_<_Tp>& a, int b)
{
    Point_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _Tp>
inline static Point_<_Tp> operator/(const Point_<_Tp>& a, float b)
{
    Point_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _Tp>
inline static Point_<_Tp> operator/(const Point_<_Tp>& a, double b)
{
    Point_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _AccTp> inline static _AccTp normL2Sqr(const Point_<int>& pt);
template <typename _AccTp> inline static _AccTp normL2Sqr(const Point_<int64>& pt);
template <typename _AccTp> inline static _AccTp normL2Sqr(const Point_<float>& pt);
template <typename _AccTp> inline static _AccTp normL2Sqr(const Point_<double>& pt);

template <> inline int normL2Sqr<int>(const Point_<int>& pt) { return pt.dot(pt); }

template <> inline int64 normL2Sqr<int64>(const Point_<int64>& pt) { return pt.dot(pt); }

template <> inline float normL2Sqr<float>(const Point_<float>& pt) { return pt.dot(pt); }

template <> inline double normL2Sqr<double>(const Point_<int>& pt) { return pt.dot(pt); }

template <> inline double normL2Sqr<double>(const Point_<float>& pt) { return pt.ddot(pt); }

template <> inline double normL2Sqr<double>(const Point_<double>& pt) { return pt.ddot(pt); }

//////////////////////////////// 3D Point ///////////////////////////////

template <typename _Tp>
inline Point3_<_Tp>::Point3_():
    x(0), y(0), z(0)
{}

template <typename _Tp>
inline Point3_<_Tp>::Point3_(_Tp _x, _Tp _y, _Tp _z):
    x(_x), y(_y), z(_z)
{}

template <typename _Tp>
inline Point3_<_Tp>::Point3_(const Point_<_Tp>& pt):
    x(pt.x), y(pt.y), z(_Tp())
{}

template <typename _Tp>
inline Point3_<_Tp>::Point3_(const Vec<_Tp, 3>& v):
    x(v[0]), y(v[1]), z(v[2])
{}

template <typename _Tp>
template <typename _Tp2>
inline Point3_<_Tp>::operator Point3_<_Tp2>() const
{
    return Point3_<_Tp2>(saturate_cast<_Tp2>(x), saturate_cast<_Tp2>(y), saturate_cast<_Tp2>(z));
}

template <typename _Tp>
inline Point3_<_Tp>::operator Vec<_Tp, 3>() const
{
    return Vec<_Tp, 3>(x, y, z);
}

template <typename _Tp>
inline _Tp Point3_<_Tp>::dot(const Point3_& pt) const
{
    return saturate_cast<_Tp>(x * pt.x + y * pt.y + z * pt.z);
}

template <typename _Tp>
inline double Point3_<_Tp>::ddot(const Point3_& pt) const
{
    return (double)x * pt.x + (double)y * pt.y + (double)z * pt.z;
}

template <typename _Tp>
inline Point3_<_Tp> Point3_<_Tp>::cross(const Point3_<_Tp>& pt) const
{
    return Point3_<_Tp>(y * pt.z - z * pt.y, z * pt.x - x * pt.z, x * pt.y - y * pt.x);
}

template <typename _Tp>
inline static Point3_<_Tp>& operator+=(Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator-=(Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator*=(Point3_<_Tp>& a, int b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    a.z = saturate_cast<_Tp>(a.z * b);
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator*=(Point3_<_Tp>& a, float b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    a.z = saturate_cast<_Tp>(a.z * b);
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator*=(Point3_<_Tp>& a, double b)
{
    a.x = saturate_cast<_Tp>(a.x * b);
    a.y = saturate_cast<_Tp>(a.y * b);
    a.z = saturate_cast<_Tp>(a.z * b);
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator/=(Point3_<_Tp>& a, int b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    a.z = saturate_cast<_Tp>(a.z / b);
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator/=(Point3_<_Tp>& a, float b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    a.z = saturate_cast<_Tp>(a.z / b);
    return a;
}

template <typename _Tp>
inline static Point3_<_Tp>& operator/=(Point3_<_Tp>& a, double b)
{
    a.x = saturate_cast<_Tp>(a.x / b);
    a.y = saturate_cast<_Tp>(a.y / b);
    a.z = saturate_cast<_Tp>(a.z / b);
    return a;
}

template <typename _Tp>
inline static double norm(const Point3_<_Tp>& pt)
{
    return std::sqrt((double)pt.x * pt.x + (double)pt.y * pt.y + (double)pt.z * pt.z);
}

template <typename _Tp>
inline static bool operator==(const Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

template <typename _Tp>
inline static bool operator!=(const Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

template <typename _Tp>
inline static Point3_<_Tp> operator+(const Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(a.x + b.x), saturate_cast<_Tp>(a.y + b.y), saturate_cast<_Tp>(a.z + b.z));
}

template <typename _Tp>
inline static Point3_<_Tp> operator-(const Point3_<_Tp>& a, const Point3_<_Tp>& b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(a.x - b.x), saturate_cast<_Tp>(a.y - b.y), saturate_cast<_Tp>(a.z - b.z));
}

template <typename _Tp>
inline static Point3_<_Tp> operator-(const Point3_<_Tp>& a)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(-a.x), saturate_cast<_Tp>(-a.y), saturate_cast<_Tp>(-a.z));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(const Point3_<_Tp>& a, int b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b), saturate_cast<_Tp>(a.z * b));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(int a, const Point3_<_Tp>& b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a), saturate_cast<_Tp>(b.z * a));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(const Point3_<_Tp>& a, float b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b), saturate_cast<_Tp>(a.z * b));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(float a, const Point3_<_Tp>& b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a), saturate_cast<_Tp>(b.z * a));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(const Point3_<_Tp>& a, double b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(a.x * b), saturate_cast<_Tp>(a.y * b), saturate_cast<_Tp>(a.z * b));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(double a, const Point3_<_Tp>& b)
{
    return Point3_<_Tp>(saturate_cast<_Tp>(b.x * a), saturate_cast<_Tp>(b.y * a), saturate_cast<_Tp>(b.z * a));
}

template <typename _Tp>
inline static Point3_<_Tp> operator*(const Matx<_Tp, 3, 3>& a, const Point3_<_Tp>& b)
{
    Matx<_Tp, 3, 1> tmp = a * Vec<_Tp, 3>(b.x, b.y, b.z);
    return Point3_<_Tp>(tmp.val[0], tmp.val[1], tmp.val[2]);
}

template <typename _Tp>
inline static Matx<_Tp, 4, 1> operator*(const Matx<_Tp, 4, 4>& a, const Point3_<_Tp>& b)
{
    return a * Matx<_Tp, 4, 1>(b.x, b.y, b.z, 1);
}

template <typename _Tp>
inline static Point3_<_Tp> operator/(const Point3_<_Tp>& a, int b)
{
    Point3_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _Tp>
inline static Point3_<_Tp> operator/(const Point3_<_Tp>& a, float b)
{
    Point3_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

template <typename _Tp>
inline static Point3_<_Tp> operator/(const Point3_<_Tp>& a, double b)
{
    Point3_<_Tp> tmp(a);
    tmp /= b;
    return tmp;
}

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

///////////////////////////////// Range /////////////////////////////////

inline Range::Range():
    start(0), end(0) {}

inline Range::Range(int _start, int _end):
    start(_start), end(_end) {}

inline int Range::size() const
{
    return end - start;
}

inline bool Range::empty() const
{
    return start == end;
}

inline Range Range::all()
{
    return Range(INT_MIN, INT_MAX);
}

inline static bool operator==(const Range& r1, const Range& r2)
{
    return r1.start == r2.start && r1.end == r2.end;
}

inline static bool operator!=(const Range& r1, const Range& r2)
{
    return !(r1 == r2);
}

inline static bool operator!(const Range& r)
{
    return r.start == r.end;
}

inline static Range operator&(const Range& r1, const Range& r2)
{
    Range r(std::max(r1.start, r2.start), std::min(r1.end, r2.end));
    r.end = std::max(r.end, r.start);
    return r;
}

inline static Range& operator&=(Range& r1, const Range& r2)
{
    r1 = r1 & r2;
    return r1;
}

inline static Range operator+(const Range& r1, int delta)
{
    return Range(r1.start + delta, r1.end + delta);
}

inline static Range operator+(int delta, const Range& r1)
{
    return Range(r1.start + delta, r1.end + delta);
}

inline static Range operator-(const Range& r1, int delta)
{
    return r1 + (-delta);
}

////////////////////////////////// Rect /////////////////////////////////

template <typename _Tp>
inline Rect_<_Tp>::Rect_():
    x(0), y(0), width(0), height(0)
{}

template <typename _Tp>
inline Rect_<_Tp>::Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height):
    x(_x), y(_y), width(_width), height(_height)
{}

template <typename _Tp>
inline Rect_<_Tp>::Rect_(const Point_<_Tp>& org, const Size_<_Tp>& sz):
    x(org.x), y(org.y), width(sz.width), height(sz.height)
{}

template <typename _Tp>
inline Rect_<_Tp>::Rect_(const Point_<_Tp>& pt1, const Point_<_Tp>& pt2)
{
    x      = std::min(pt1.x, pt2.x);
    y      = std::min(pt1.y, pt2.y);
    width  = std::max(pt1.x, pt2.x) - x;
    height = std::max(pt1.y, pt2.y) - y;
}

template <typename _Tp>
inline Point_<_Tp> Rect_<_Tp>::tl() const
{
    return Point_<_Tp>(x, y);
}

template <typename _Tp>
inline Point_<_Tp> Rect_<_Tp>::br() const
{
    return Point_<_Tp>(x + width, y + height);
}

template <typename _Tp>
inline Size_<_Tp> Rect_<_Tp>::size() const
{
    return Size_<_Tp>(width, height);
}

template <typename _Tp>
inline _Tp Rect_<_Tp>::area() const
{
    const _Tp result = width * height;
    HL_DbgAssert(!std::numeric_limits<_Tp>::is_integer
                 || width == 0 || result / width == height);    // make sure the result fits in the return value
    return result;
}

template <typename _Tp>
inline bool Rect_<_Tp>::empty() const
{
    return width <= 0 || height <= 0;
}

template <typename _Tp>
template <typename _Tp2>
inline Rect_<_Tp>::operator Rect_<_Tp2>() const
{
    return Rect_<_Tp2>(saturate_cast<_Tp2>(x), saturate_cast<_Tp2>(y), saturate_cast<_Tp2>(width), saturate_cast<_Tp2>(height));
}

template <typename _Tp>
inline bool Rect_<_Tp>::contains(const Point_<_Tp>& pt) const
{
    return x <= pt.x && pt.x < x + width && y <= pt.y && pt.y < y + height;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator+=(Rect_<_Tp>& a, const Point_<_Tp>& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator-=(Rect_<_Tp>& a, const Point_<_Tp>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator+=(Rect_<_Tp>& a, const Size_<_Tp>& b)
{
    a.width  += b.width;
    a.height += b.height;
    return a;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator-=(Rect_<_Tp>& a, const Size_<_Tp>& b)
{
    const _Tp width  = a.width - b.width;
    const _Tp height = a.height - b.height;
    HL_DbgAssert(width >= 0 && height >= 0);
    a.width  = width;
    a.height = height;
    return a;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator&=(Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    if (a.empty() || b.empty())
    {
        a = Rect();
        return a;
    }
    const Rect_<_Tp>& Rx_min = (a.x < b.x) ? a : b;
    const Rect_<_Tp>& Rx_max = (a.x < b.x) ? b : a;
    const Rect_<_Tp>& Ry_min = (a.y < b.y) ? a : b;
    const Rect_<_Tp>& Ry_max = (a.y < b.y) ? b : a;
    // Looking at the formula below, we will compute Rx_min.width - (Rx_max.x - Rx_min.x)
    // but we want to avoid overflows. Rx_min.width >= 0 and (Rx_max.x - Rx_min.x) >= 0
    // by definition so the difference does not overflow. The only thing that can overflow
    // is (Rx_max.x - Rx_min.x). And it can only overflow if Rx_min.x < 0.
    // Let us first deal with the following case.
    if ((Rx_min.x < 0 && Rx_min.x + Rx_min.width < Rx_max.x) || (Ry_min.y < 0 && Ry_min.y + Ry_min.height < Ry_max.y))
    {
        a = Rect();
        return a;
    }
    // We now know that either Rx_min.x >= 0, or
    // Rx_min.x < 0 && Rx_min.x + Rx_min.width >= Rx_max.x and therefore
    // Rx_min.width >= (Rx_max.x - Rx_min.x) which means (Rx_max.x - Rx_min.x)
    // is inferior to a valid int and therefore does not overflow.
    a.width  = std::min(Rx_min.width - (Rx_max.x - Rx_min.x), Rx_max.width);
    a.height = std::min(Ry_min.height - (Ry_max.y - Ry_min.y), Ry_max.height);
    a.x      = Rx_max.x;
    a.y      = Ry_max.y;
    if (a.empty())
        a = Rect();
    return a;
}

template <typename _Tp>
inline static Rect_<_Tp>& operator|=(Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    if (a.empty())
    {
        a = b;
    }
    else if (!b.empty())
    {
        _Tp x1   = std::min(a.x, b.x);
        _Tp y1   = std::min(a.y, b.y);
        a.width  = std::max(a.x + a.width, b.x + b.width) - x1;
        a.height = std::max(a.y + a.height, b.y + b.height) - y1;
        a.x      = x1;
        a.y      = y1;
    }
    return a;
}

template <typename _Tp>
inline static bool operator==(const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

template <typename _Tp>
inline static bool operator!=(const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
}

template <typename _Tp>
inline static Rect_<_Tp> operator+(const Rect_<_Tp>& a, const Point_<_Tp>& b)
{
    return Rect_<_Tp>(a.x + b.x, a.y + b.y, a.width, a.height);
}

template <typename _Tp>
inline static Rect_<_Tp> operator-(const Rect_<_Tp>& a, const Point_<_Tp>& b)
{
    return Rect_<_Tp>(a.x - b.x, a.y - b.y, a.width, a.height);
}

template <typename _Tp>
inline static Rect_<_Tp> operator+(const Rect_<_Tp>& a, const Size_<_Tp>& b)
{
    return Rect_<_Tp>(a.x, a.y, a.width + b.width, a.height + b.height);
}

template <typename _Tp>
inline static Rect_<_Tp> operator-(const Rect_<_Tp>& a, const Size_<_Tp>& b)
{
    const _Tp width  = a.width - b.width;
    const _Tp height = a.height - b.height;
    HL_DbgAssert(width >= 0 && height >= 0);
    return Rect_<_Tp>(a.x, a.y, width, height);
}

template <typename _Tp>
inline static Rect_<_Tp> operator&(const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    Rect_<_Tp> c  = a;
    return c     &= b;
}

template <typename _Tp>
inline static Rect_<_Tp> operator|(const Rect_<_Tp>& a, const Rect_<_Tp>& b)
{
    Rect_<_Tp> c  = a;
    return c     |= b;
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
