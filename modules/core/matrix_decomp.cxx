#include "precomp.hxx"

namespace hl
{
namespace hal
{

/****************************************************************************************\
*                     LU & Cholesky implementation for small matrices                    *
\****************************************************************************************/

template <typename _Tp>
inline static int LUImpl(_Tp* A, size_t astep, int m, _Tp* b, size_t bstep, int n, _Tp eps)
{
    int i, j, k, p = 1;
    astep /= sizeof(A[0]);
    bstep /= sizeof(b[0]);

    for (i = 0; i < m; i++)
    {
        k = i;

        for (j = i + 1; j < m; j++)
            if (std::abs(A[j * astep + i]) > std::abs(A[k * astep + i]))
                k = j;

        if (std::abs(A[k * astep + i]) < eps)
            return 0;

        if (k != i)
        {
            for (j = i; j < m; j++)
                std::swap(A[i * astep + j], A[k * astep + j]);
            if (b)
                for (j = 0; j < n; j++)
                    std::swap(b[i * bstep + j], b[k * bstep + j]);
            p = -p;
        }

        _Tp d = -1 / A[i * astep + i];

        for (j = i + 1; j < m; j++)
        {
            _Tp alpha = A[j * astep + i] * d;

            for (k = i + 1; k < m; k++)
                A[j * astep + k] += alpha * A[i * astep + k];

            if (b)
                for (k = 0; k < n; k++)
                    b[j * bstep + k] += alpha * b[i * bstep + k];
        }
    }

    if (b)
    {
        for (i = m - 1; i >= 0; i--)
            for (j = 0; j < n; j++)
            {
                _Tp s = b[i * bstep + j];
                for (k = i + 1; k < m; k++)
                    s -= A[i * astep + k] * b[k * bstep + j];
                b[i * bstep + j] = s / A[i * astep + i];
            }
    }

    return p;
}

int LU32f(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    int output;
    output = LUImpl(A, astep, m, b, bstep, n, FLT_EPSILON * 10);
    return output;
}

int LU64f(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    int output;
    output = LUImpl(A, astep, m, b, bstep, n, DBL_EPSILON * 100);
    return output;
}

template <typename _Tp>
inline static bool CholImpl(_Tp* A, size_t astep, int m, _Tp* b, size_t bstep, int n)
{
    _Tp*   L = A;
    int    i, j, k;
    double s;
    astep /= sizeof(A[0]);
    bstep /= sizeof(b[0]);

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < i; j++)
        {
            s = A[i * astep + j];
            for (k = 0; k < j; k++)
                s -= L[i * astep + k] * L[j * astep + k];
            L[i * astep + j] = (_Tp)(s * L[j * astep + j]);
        }
        s = A[i * astep + i];
        for (k = 0; k < j; k++)
        {
            double t  = L[i * astep + k];
            s        -= t * t;
        }
        if (s < std::numeric_limits<_Tp>::epsilon())
            return false;
        L[i * astep + i] = (_Tp)(1. / std::sqrt(s));
    }

    if (!b)
    {
        for (i = 0; i < m; i++)
            L[i * astep + i] = 1 / L[i * astep + i];
        return true;
    }

    // LLt x = b
    // 1: L y = b
    // 2. Lt x = y

    /*
     [ L00             ]  y0   b0
     [ L10 L11         ]  y1 = b1
     [ L20 L21 L22     ]  y2   b2
     [ L30 L31 L32 L33 ]  y3   b3

     [ L00 L10 L20 L30 ]  x0   y0
     [     L11 L21 L31 ]  x1 = y1
     [         L22 L32 ]  x2   y2
     [             L33 ]  x3   y3
     */

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            s = b[i * bstep + j];
            for (k = 0; k < i; k++)
                s -= L[i * astep + k] * b[k * bstep + j];
            b[i * bstep + j] = (_Tp)(s * L[i * astep + i]);
        }
    }

    for (i = m - 1; i >= 0; i--)
    {
        for (j = 0; j < n; j++)
        {
            s = b[i * bstep + j];
            for (k = m - 1; k > i; k--)
                s -= L[k * astep + i] * b[k * bstep + j];
            b[i * bstep + j] = (_Tp)(s * L[i * astep + i]);
        }
    }
    for (i = 0; i < m; i++)
        L[i * astep + i] = 1 / L[i * astep + i];

    return true;
}

bool Cholesky32f(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    return CholImpl(A, astep, m, b, bstep, n);
}

bool Cholesky64f(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    return CholImpl(A, astep, m, b, bstep, n);
}

//=============================================================================
// for compatibility with 3.0

int LU(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    return LUImpl(A, astep, m, b, bstep, n, FLT_EPSILON * 10);
}

int LU(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    return LUImpl(A, astep, m, b, bstep, n, DBL_EPSILON * 100);
}

bool Cholesky(float* A, size_t astep, int m, float* b, size_t bstep, int n)
{
    return CholImpl(A, astep, m, b, bstep, n);
}

bool Cholesky(double* A, size_t astep, int m, double* b, size_t bstep, int n)
{
    return CholImpl(A, astep, m, b, bstep, n);
}

}    // namespace hal
}    // namespace hl