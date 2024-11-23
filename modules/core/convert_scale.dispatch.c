#include "precomp.h"
#include "convert_scale.simd.h"

BinaryFunc getConvertScaleFunc(int sdepth, int ddepth)
{
    return convertScaleFunc_cpu_baseline(sdepth, ddepth);
}
