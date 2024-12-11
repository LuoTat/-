#pragma once

#include "openHL/core/hldef.h"

namespace hl
{

template <typename _Tp> class DataType
{
public:
};

template <> class DataType<bool>
{
public:
    typedef bool       value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_8U,
        channels     = 1,
        fmt          = (int)'u',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<uchar>
{
public:
    typedef uchar      value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_8U,
        channels     = 1,
        fmt          = (int)'u',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<schar>
{
public:
    typedef schar      value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_8S,
        channels     = 1,
        fmt          = (int)'c',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<char>
{
public:
    typedef schar      value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_8S,
        channels     = 1,
        fmt          = (int)'c',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<ushort>
{
public:
    typedef ushort     value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_16U,
        channels     = 1,
        fmt          = (int)'w',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<short>
{
public:
    typedef short      value_type;
    typedef int        work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_16S,
        channels     = 1,
        fmt          = (int)'s',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<uint>
{
public:
    typedef int        value_type;
    typedef value_type work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_32U,
        channels     = 1,
        fmt          = (int)'j',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<int>
{
public:
    typedef int        value_type;
    typedef value_type work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_32S,
        channels     = 1,
        fmt          = (int)'i',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<float>
{
public:
    typedef float      value_type;
    typedef value_type work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_32F,
        channels     = 1,
        fmt          = (int)'f',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <> class DataType<double>
{
public:
    typedef double     value_type;
    typedef value_type work_type;
    typedef value_type channel_type;
    typedef value_type vec_type;

    enum
    {
        generic_type = 0,
        depth        = HL_64F,
        channels     = 1,
        fmt          = (int)'d',
        type         = HL_MAKETYPE(depth, channels)
    };
};

template <typename _Tp> class DataDepth
{
public:
    enum
    {
        value = DataType<_Tp>::depth,
        fmt   = DataType<_Tp>::fmt
    };
};

namespace traits
{

namespace internal
{
#define HL_CREATE_MEMBER_CHECK(X)                                                  \
    template <typename T> class CheckMember_##X                                    \
    {                                                                              \
        struct Fallback                                                            \
        {                                                                          \
            int X;                                                                 \
        };                                                                         \
        struct Derived: T, Fallback                                                \
        {};                                                                        \
        template <typename U, U> struct Check;                                     \
        typedef char                         HL_NO[1];                             \
        typedef char                         HL_YES[2];                            \
        template <typename U> static HL_NO&  func(Check<int Fallback::*, &U::X>*); \
        template <typename U> static HL_YES& func(...);                            \
                                                                                   \
    public:                                                                        \
        typedef CheckMember_##X type;                                              \
        enum                                                                       \
        {                                                                          \
            value = sizeof(func<Derived>(0)) == sizeof(HL_YES)                     \
        };                                                                         \
    };

HL_CREATE_MEMBER_CHECK(fmt)
HL_CREATE_MEMBER_CHECK(type)

}    // namespace internal

template <typename T>
struct Depth
{
    enum
    {
        value = DataType<T>::depth
    };
};

template <typename T>
struct Type
{
    enum
    {
        value = DataType<T>::type
    };
};

/** Similar to traits::Type<T> but has value = -1 in case of unknown type (instead of compiler error) */
template <typename T, bool available = internal::CheckMember_type<DataType<T>>::value>
struct SafeType
{};

template <typename T>
struct SafeType<T, false>
{
    enum
    {
        value = -1
    };
};

template <typename T>
struct SafeType<T, true>
{
    enum
    {
        value = Type<T>::value
    };
};

template <typename T, bool available = internal::CheckMember_fmt<DataType<T>>::value>
struct SafeFmt
{};

template <typename T>
struct SafeFmt<T, false>
{
    enum
    {
        fmt = 0
    };
};

template <typename T>
struct SafeFmt<T, true>
{
    enum
    {
        fmt = DataType<T>::fmt
    };
};

}    // namespace traits
}    // namespace hl