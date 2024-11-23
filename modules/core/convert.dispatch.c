#include "precomp.h"
#include "convert.simd.h"

BinaryFunc getConvertFunc(int sdepth, int ddepth)
{
    return getConvertFunc_cpu_baseline(sdepth, ddepth);
}

void convertTo(const Mat* src, Mat* dst, int rtype, double alpha, double beta)
{
    if (empty(src))
    {
        release(dst);
        return;
    }
    int stype  = type(src);
    int sdepth = HL_MAT_DEPTH(stype);

    int ddepth = sdepth;
    if (rtype >= 0)
        ddepth = HL_MAT_DEPTH(rtype);
    else
        ddepth = depth(dst);

    bool noScale = fabs(alpha - 1) < DBL_EPSILON && fabs(beta) < DBL_EPSILON;
    if (sdepth == ddepth && noScale)
    {
        copyTo(src, dst);
        return;
    }

    int cn    = channels(src);
    int dtype = HL_MAKETYPE(ddepth, cn);

    create(dst, src->cols, src->rows, dtype);

    BinaryFunc func = noScale ? getConvertFunc(sdepth, ddepth) : getConvertScaleFunc(sdepth, ddepth);

    double scale[]  = {alpha, beta};
    assert(func != 0);

    func(src->data, src->step[0], 0, 0, dst->data, dst->step[0], dst->cols, dst->rows, scale);
}