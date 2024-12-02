#include "precomp.hxx"
#include <vector>
#include "median_blur.simd.hxx"

namespace hl
{
void medianBlur(const Mat& _src0, Mat& _dst, int ksize)
{
    HL_Assert(!_src0.empty());

    HL_Assert((ksize % 2 == 1) && (_src0.dims <= 2));

    if (ksize <= 1 || _src0.empty())
    {
        _src0.copyTo(_dst);
        return;
    }

    Mat src0 = _src0;
    _dst.create(src0.size(), src0.type());
    Mat dst = _dst;

    cpu_baseline::medianBlur(src0, dst, ksize);
}
}    // namespace hl