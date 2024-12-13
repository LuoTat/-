#include "precomp.hxx"
#include <limits>

namespace hl
{

int LU(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    return hal::LU32f(A, astep, m, b, bstep, n);
}

int LU(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    return hal::LU64f(A, astep, m, b, bstep, n);
}

bool Cholesky(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    return hal::Cholesky32f(A, astep, m, b, bstep, n);
}

bool Cholesky(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    return hal::Cholesky64f(A, astep, m, b, bstep, n);
}

}    // namespace hl