#include "precomp.hxx"
#include "convert.simd.hxx"

namespace hl
{

BinaryFunc getConvertFunc(int sdepth, int ddepth)
{
    return cpu_baseline ::getConvertFunc(sdepth, ddepth);
}

void Mat::convertTo(Mat& _dst, int _type, double alpha, double beta) const
{
    if (empty())
    {
        _dst.release();
        return;
    }

    bool noScale = fabs(alpha - 1) < DBL_EPSILON && fabs(beta) < DBL_EPSILON;

    if (_type < 0)
        _type = _dst.type();
    else
        _type = HL_MAKETYPE(HL_MAT_DEPTH(_type), channels());

    int sdepth = depth(), ddepth = HL_MAT_DEPTH(_type);
    if (sdepth == ddepth && noScale)
    {
        copyTo(_dst);
        return;
    }

    Mat src = *this;
    if (dims <= 2)
        _dst.create(size(), _type);
    else
        _dst.create(dims, size, _type);
    Mat dst            = _dst;

    BinaryFunc func    = noScale ? getConvertFunc(sdepth, ddepth) : getConvertScaleFunc(sdepth, ddepth);
    double     scale[] = {alpha, beta};
    int        cn      = channels();
    HL_Assert(func != 0);

    if (dims <= 2)
    {
        Size sz = getContinuousSize2D(src, dst, cn);
        func(src.data, src.step, 0, 0, dst.data, dst.step, sz, scale);
    }
    else
    {
        const Mat*      arrays[] = {&src, &dst, 0};
        uchar*          ptrs[2]  = {};
        NAryMatIterator it(arrays, ptrs);
        Size            sz((int)(it.size * cn), 1);

        for (size_t i = 0; i < it.nplanes; i++, ++it)
            func(ptrs[0], 1, 0, 0, ptrs[1], 1, sz, scale);
    }
}

}    // namespace hl
