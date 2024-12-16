#include "openHL/core/hal/intrin.hxx"

namespace hl
{
namespace hal
{

extern const uchar popCountTable[256];

namespace cpu_baseline
{

// forward declarations
int normHamming(const uchar* a, int n);
int normHamming(const uchar* a, const uchar* b, int n);

int normHamming(const uchar* a, int n)
{
    int i      = 0;
    int result = 0;

    for (; i <= n - 4; i += 4)
    {
        result += popCountTable[a[i]] + popCountTable[a[i + 1]] + popCountTable[a[i + 2]] + popCountTable[a[i + 3]];
    }
    for (; i < n; i++)
    {
        result += popCountTable[a[i]];
    }
    return result;
}

int normHamming(const uchar* a, const uchar* b, int n)
{
    int i      = 0;
    int result = 0;

    for (; i <= n - 4; i += 4)
    {
        result += popCountTable[a[i] ^ b[i]] + popCountTable[a[i + 1] ^ b[i + 1]] + popCountTable[a[i + 2] ^ b[i + 2]] + popCountTable[a[i + 3] ^ b[i + 3]];
    }
    for (; i < n; i++)
    {
        result += popCountTable[a[i] ^ b[i]];
    }
    return result;
}


}    // namespace cpu_baseline
}    // namespace hal
}    // namespace hl
