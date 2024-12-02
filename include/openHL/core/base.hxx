#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.hxx"
#include <stdexcept>
#include <format>

namespace hl
{
namespace Error
{
enum Code
{
    StsOk                     = 0,
    StsBackTrace              = -1,
    StsError                  = -2,
    StsInternal               = -3,
    StsNoMem                  = -4,
    StsBadArg                 = -5,
    StsBadFunc                = -6,
    StsNoConv                 = -7,
    StsAutoTrace              = -8,
    HeaderIsNull              = -9,
    BadImageSize              = -10,
    BadOffset                 = -11,
    BadDataPtr                = -12,
    BadStep                   = -13,
    BadModelOrChSeq           = -14,
    BadNumChannels            = -15,
    BadNumChannel1U           = -16,
    BadDepth                  = -17,
    BadAlphaChannel           = -18,
    BadOrder                  = -19,
    BadOrigin                 = -20,
    BadAlign                  = -21,
    BadCallBack               = -22,
    BadTileSize               = -23,
    BadCOI                    = -24,
    BadROISize                = -25,
    MaskIsTiled               = -26,
    StsNullPtr                = -27,
    StsVecLengthErr           = -28,
    StsFilterStructContentErr = -29,
    StsKernelStructContentErr = -30,
    StsFilterOffsetErr        = -31,
    StsBadSize                = -201,
    StsDivByZero              = -202,
    StsInplaceNotSupported    = -203,
    StsObjectNotFound         = -204,
    StsUnmatchedFormats       = -205,
    StsBadFlag                = -206,
    StsBadPoint               = -207,
    StsBadMask                = -208,
    StsUnmatchedSizes         = -209,
    StsUnsupportedFormat      = -210,
    StsOutOfRange             = -211,
    StsParseError             = -212,
    StsNotImplemented         = -213,
    StsBadMemBlock            = -214,
    StsAssert                 = -215,
    GpuNotSupported           = -216,
    GpuApiCallError           = -217,
    OpenGlNotSupported        = -218,
    OpenGlApiCallError        = -219,
    OpenCLApiCallError        = -220,
    OpenCLDoubleNotSupported  = -221,
    OpenCLInitError           = -222,
    OpenCLNoAMDBlasFft        = -223
};
}    // namespace Error

enum NormTypes
{
    NORM_INF       = 1,
    NORM_L1        = 2,
    NORM_L2        = 4,
    NORM_L2SQR     = 5,
    NORM_HAMMING   = 6,
    NORM_HAMMING2  = 7,
    NORM_TYPE_MASK = 7,
    NORM_RELATIVE  = 8,
    NORM_MINMAX    = 32
};

enum BorderTypes
{
    BORDER_CONSTANT    = 0,
    BORDER_REPLICATE   = 1,
    BORDER_REFLECT     = 2,
    BORDER_WRAP        = 3,
    BORDER_REFLECT_101 = 4,
    BORDER_TRANSPARENT = 5,

    BORDER_REFLECT101  = BORDER_REFLECT_101,
    BORDER_DEFAULT     = BORDER_REFLECT_101,
    BORDER_ISOLATED    = 16
};

[[noreturn]]
void error(int _code, const String& _err, const char* _func, const char* _file, int _line);

#define HL_Error(code, msg)   hl::error(code, msg, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define HL_Error_(code, args) hl::error(code, std::format args, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define HL_Assert(expr)                                                                       \
    do {                                                                                      \
        if (!!(expr))                                                                         \
            ;                                                                                 \
        else hl::error(hl::Error::StsAssert, #expr, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
    }                                                                                         \
    while (0)



#ifdef _DEBUG
    #define HL_DbgAssert(expr) HL_Assert(expr)
#else
    #define HL_DbgAssert(expr)
#endif




////////////////// forward declarations for important OpenCV types //////////////////

//! @cond IGNORED

template <typename _Tp, int cn> class Vec;
template <typename _Tp, int m, int n> class Matx;

template <typename _Tp> class Complex;
template <typename _Tp> class Point_;
template <typename _Tp> class Point3_;
template <typename _Tp> class Size_;
template <typename _Tp> class Rect_;
template <typename _Tp> class Scalar_;
}    // namespace hl

#include "openHL/core/check.hxx"