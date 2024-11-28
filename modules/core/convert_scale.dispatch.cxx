#include "precomp.hxx"
#include "convert_scale.simd.hxx"

namespace hl
{
BinaryFunc getConvertScaleFunc(int sdepth, int ddepth)
{
    return cpu_baseline::getConvertScaleFunc(sdepth, ddepth);
}
}    // namespace hl
