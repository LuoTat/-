#include "precomp.hxx"

namespace hl
{

void normalize(const Mat& _src, Mat& _dst, double a, double b, int norm_type, int rtype, const Mat& _mask)
{
    double scale = 1, shift = 0;

    if (rtype < 0)
        rtype = _dst.depth();

    if (norm_type == HL_MINMAX)
    {
        double smin = 0, smax = 0;
        double dmin = MIN(a, b), dmax = MAX(a, b);
        minMaxIdx(_src, &smin, &smax, 0, 0, _mask);
        scale = (dmax - dmin) * (smax - smin > DBL_EPSILON ? 1. / (smax - smin) : 0);
        if (rtype == HL_32F)
        {
            scale = (float)scale;
            shift = (float)dmin - (float)(smin * scale);
        }
        else
            shift = dmin - smin * scale;
    }
    else if (norm_type == HL_L2 || norm_type == HL_L1 || norm_type == HL_C)
    {
        // TODO: implement norm support
        // scale = norm(_src, norm_type, _mask);
        scale = scale > DBL_EPSILON ? a / scale : 0.;
        shift = 0;
    }
    else
        HL_Error(HL_StsBadArg, "Unknown/unsupported norm type");

    Mat src = _src;
    if (_mask.empty())
        src.convertTo(_dst, rtype, scale, shift);
    else
    {
        Mat temp;
        src.convertTo(temp, rtype, scale, shift);
        temp.copyTo(_dst, _mask);
    }
}
}    // namespace hl