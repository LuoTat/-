#pragma once

#include "openHL/imgproc.hxx"

namespace hl
{

enum
{
    KERNEL_GENERAL      = 0,
    KERNEL_SYMMETRICAL  = 1,
    KERNEL_ASYMMETRICAL = 2,
    KERNEL_SMOOTH       = 4,
    KERNEL_INTEGER      = 8
};

class BaseRowFilter
{
public:
    //! the default constructor
    BaseRowFilter();
    //! the destructor
    virtual ~BaseRowFilter();
    //! the filtering operator. Must be overridden in the derived classes. The horizontal border interpolation is done outside of the class.
    virtual void operator()(const uchar* src, uchar* dst, int width, int cn) = 0;

    int ksize;
    int anchor;
};

class BaseColumnFilter
{
public:
    //! the default constructor
    BaseColumnFilter();
    //! the destructor
    virtual ~BaseColumnFilter();
    //! the filtering operator. Must be overridden in the derived classes. The vertical border interpolation is done outside of the class.
    virtual void operator()(const uchar** src, uchar* dst, int dststep, int dstcount, int width) = 0;
    //! resets the internal buffers, if any
    virtual void reset();

    int ksize;
    int anchor;
};

class BaseFilter
{
public:
    //! the default constructor
    BaseFilter();
    //! the destructor
    virtual ~BaseFilter();
    //! the filtering operator. The horizontal and the vertical border interpolation is done outside of the class.
    virtual void operator()(const uchar** src, uchar* dst, int dststep, int dstcount, int width, int cn) = 0;
    //! resets the internal buffers, if any
    virtual void reset();

    Size  ksize;
    Point anchor;
};

class FilterEngine
{
public:
    //! the default constructor
    FilterEngine();
    //! the full constructor. Either _filter2D or both _rowFilter and _columnFilter must be non-empty.
    FilterEngine(const Ptr<BaseFilter>&       _filter2D,
                 const Ptr<BaseRowFilter>&    _rowFilter,
                 const Ptr<BaseColumnFilter>& _columnFilter,
                 int                          srcType,
                 int                          dstType,
                 int                          bufType,
                 int                          _rowBorderType    = BORDER_REPLICATE,
                 int                          _columnBorderType = -1,
                 const Scalar&                _borderValue      = Scalar());
    //! the destructor
    virtual ~FilterEngine();
    //! reinitializes the engine. The previously assigned filters are released.
    void init(const Ptr<BaseFilter>&       _filter2D,
              const Ptr<BaseRowFilter>&    _rowFilter,
              const Ptr<BaseColumnFilter>& _columnFilter,
              int                          srcType,
              int                          dstType,
              int                          bufType,
              int                          _rowBorderType    = BORDER_REPLICATE,
              int                          _columnBorderType = -1,
              const Scalar&                _borderValue      = Scalar());

    //! starts filtering of the specified ROI of an image of size wholeSize.
    virtual int start(const hl::Size& wholeSize, const hl::Size& sz, const hl::Point& ofs);
    //! starts filtering of the specified ROI of the specified image.
    virtual int start(const Mat& src, const hl::Size& wsz, const hl::Point& ofs);
    //! processes the next srcCount rows of the image.
    virtual int proceed(const uchar* src, int srcStep, int srcCount, uchar* dst, int dstStep);
    //! applies filter to the specified ROI of the image. if srcRoi=(0,0,-1,-1), the whole image is filtered.
    virtual void apply(const Mat& src, Mat& dst, const hl::Size& wsz, const hl::Point& ofs);

    //! returns true if the filter is separable
    bool isSeparable() const { return !filter2D; }

    //! returns the number
    int remainingInputRows() const;
    int remainingOutputRows() const;

    int                 srcType;
    int                 dstType;
    int                 bufType;
    Size                ksize;
    Point               anchor;
    int                 maxWidth;
    Size                wholeSize;
    Rect                roi;
    int                 dx1;
    int                 dx2;
    int                 rowBorderType;
    int                 columnBorderType;
    std::vector<int>    borderTab;
    int                 borderElemSize;
    std::vector<uchar>  ringBuf;
    std::vector<uchar>  srcRow;
    std::vector<uchar>  constBorderValue;
    std::vector<uchar>  constBorderRow;
    int                 bufStep;
    int                 startY;
    int                 startY0;
    int                 endY;
    int                 rowCount;
    int                 dstY;
    std::vector<uchar*> rows;

    Ptr<BaseFilter>       filter2D;
    Ptr<BaseRowFilter>    rowFilter;
    Ptr<BaseColumnFilter> columnFilter;
};

}    // namespace hl