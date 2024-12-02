#include "precomp.hxx"
// #include "filter.hxx"
#include "filter.simd.hxx"

/****************************************************************************************\
                                    Base Image Filter
\****************************************************************************************/

namespace hl
{
BaseRowFilter::BaseRowFilter() { ksize = anchor = -1; }

BaseRowFilter::~BaseRowFilter() {}

BaseColumnFilter::BaseColumnFilter() { ksize = anchor = -1; }

BaseColumnFilter::~BaseColumnFilter() {}

void BaseColumnFilter::reset() {}

BaseFilter::BaseFilter()
{
    ksize  = Size(-1, -1);
    anchor = Point(-1, -1);
}

BaseFilter::~BaseFilter() {}

void BaseFilter::reset() {}

FilterEngine::FilterEngine():
    srcType(-1), dstType(-1), bufType(-1), maxWidth(0), wholeSize(-1, -1), dx1(0), dx2(0), rowBorderType(BORDER_REPLICATE), columnBorderType(BORDER_REPLICATE), borderElemSize(0), bufStep(0), startY(0), startY0(0), endY(0), rowCount(0), dstY(0)
{
}

FilterEngine::FilterEngine(const Ptr<BaseFilter>& _filter2D, const Ptr<BaseRowFilter>& _rowFilter, const Ptr<BaseColumnFilter>& _columnFilter, int _srcType, int _dstType, int _bufType, int _rowBorderType, int _columnBorderType, const Scalar& _borderValue):
    srcType(-1), dstType(-1), bufType(-1), maxWidth(0), wholeSize(-1, -1), dx1(0), dx2(0), rowBorderType(BORDER_REPLICATE), columnBorderType(BORDER_REPLICATE), borderElemSize(0), bufStep(0), startY(0), startY0(0), endY(0), rowCount(0), dstY(0)
{
    init(_filter2D, _rowFilter, _columnFilter, _srcType, _dstType, _bufType, _rowBorderType, _columnBorderType, _borderValue);
}

FilterEngine::~FilterEngine()
{
}

void FilterEngine::init(const Ptr<BaseFilter>& _filter2D, const Ptr<BaseRowFilter>& _rowFilter, const Ptr<BaseColumnFilter>& _columnFilter, int _srcType, int _dstType, int _bufType, int _rowBorderType, int _columnBorderType, const Scalar& _borderValue)
{
    _srcType        = HL_MAT_TYPE(_srcType);
    _bufType        = HL_MAT_TYPE(_bufType);
    _dstType        = HL_MAT_TYPE(_dstType);

    srcType         = _srcType;
    int srcElemSize = (int)getElemSize(srcType);
    dstType         = _dstType;
    bufType         = _bufType;

    filter2D        = _filter2D;
    rowFilter       = _rowFilter;
    columnFilter    = _columnFilter;

    if (_columnBorderType < 0)
        _columnBorderType = _rowBorderType;

    rowBorderType    = _rowBorderType;
    columnBorderType = _columnBorderType;

    HL_Assert(columnBorderType != BORDER_WRAP);

    if (isSeparable())
    {
        HL_Assert(rowFilter && columnFilter);
        ksize  = Size(rowFilter->ksize, columnFilter->ksize);
        anchor = Point(rowFilter->anchor, columnFilter->anchor);
    }
    else
    {
        HL_Assert(bufType == srcType);
        ksize  = filter2D->ksize;
        anchor = filter2D->anchor;
    }

    HL_Assert(0 <= anchor.x && anchor.x < ksize.width && 0 <= anchor.y && anchor.y < ksize.height);

    borderElemSize   = srcElemSize / (HL_MAT_DEPTH(srcType) >= HL_32S ? sizeof(int) : 1);
    int borderLength = std::max(ksize.width - 1, 1);
    borderTab.resize(borderLength * borderElemSize);

    maxWidth = bufStep = 0;
    constBorderRow.clear();

    if (rowBorderType == BORDER_CONSTANT || columnBorderType == BORDER_CONSTANT)
    {
        constBorderValue.resize(srcElemSize * borderLength);
        int srcType1 = HL_MAKETYPE(HL_MAT_DEPTH(srcType), MIN(HL_MAT_CN(srcType), 4));
        scalarToRawData(_borderValue, &constBorderValue[0], srcType1, borderLength * HL_MAT_CN(srcType));
    }

    wholeSize = Size(-1, -1);
}

int FilterEngine::start(const Size& _wholeSize, const Size& sz, const Point& ofs)
{
    HL_Assert(!sz.empty());
    HL_Assert(!_wholeSize.empty());

    return cpu_baseline::FilterEngine__start(*this, _wholeSize, sz, ofs);
}

int FilterEngine::start(const Mat& src, const Size& wsz, const Point& ofs)
{
    HL_Assert(!src.empty());
    HL_Assert(!wsz.empty());

    start(wsz, src.size(), ofs);
    return startY - ofs.y;
}

int FilterEngine::remainingInputRows() const
{
    return endY - startY - rowCount;
}

int FilterEngine::remainingOutputRows() const
{
    return roi.height - dstY;
}

int FilterEngine::proceed(const uchar* src, int srcstep, int count, uchar* dst, int dststep)
{
    HL_Assert(wholeSize.width > 0 && wholeSize.height > 0);

    return cpu_baseline::FilterEngine__proceed(*this, src, srcstep, count, dst, dststep);
}

void FilterEngine::apply(const Mat& src, Mat& dst, const Size& wsz, const Point& ofs)
{
    HL_CheckTypeEQ(src.type(), srcType, "");
    HL_CheckTypeEQ(dst.type(), dstType, "");

    cpu_baseline ::FilterEngine__apply(*this, src, dst, wsz, ofs);
}
}    // namespace hl