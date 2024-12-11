#include "precomp.hxx"
#include <vector>
#include "box_filter.simd.hxx"

namespace hl
{

Ptr<FilterEngine> createBoxFilter(int srcType, int dstType, Size ksize, Point anchor, bool normalize, int borderType)
{
    return cpu_baseline::createBoxFilter(srcType, dstType, ksize, anchor, normalize, borderType);
}

void boxFilter(const Mat& _src, Mat& _dst, int ddepth, Size ksize, Point anchor, bool normalize, int borderType)
{
    HL_Assert(!_src.empty());

    Mat src   = _src;
    int stype = src.type(), sdepth = HL_MAT_DEPTH(stype), cn = HL_MAT_CN(stype);
    if (ddepth < 0)
        ddepth = sdepth;
    _dst.create(src.size(), HL_MAKETYPE(ddepth, cn));
    Mat dst = _dst;
    if (borderType != BORDER_CONSTANT && normalize && (borderType & BORDER_ISOLATED) != 0)
    {
        if (src.rows == 1)
            ksize.height = 1;
        if (src.cols == 1)
            ksize.width = 1;
    }

    Point ofs;
    Size  wsz(src.cols, src.rows);
    if (!(borderType & BORDER_ISOLATED))
        src.locateROI(wsz, ofs);

    borderType          = (borderType & ~BORDER_ISOLATED);

    Ptr<FilterEngine> f = createBoxFilter(src.type(), dst.type(), ksize, anchor, normalize, borderType);

    f->apply(src, dst, wsz, ofs);
}

void blur(const Mat& src, Mat& dst, Size ksize, Point anchor, int borderType)
{
    boxFilter(src, dst, -1, ksize, anchor, true, borderType);
}

}    // namespace hl