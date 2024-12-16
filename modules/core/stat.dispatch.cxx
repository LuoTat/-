#include "precomp.hxx"

#include "stat.simd.hxx"

namespace hl
{
namespace hal
{

int normHamming(const uchar* a, int n)
{
    return cpu_baseline::normHamming(a, n);
}

int normHamming(const uchar* a, const uchar* b, int n)
{
    return cpu_baseline::normHamming(a, b, n);
}

}    // namespace hal
}    // namespace hl
