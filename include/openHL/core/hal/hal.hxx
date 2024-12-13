#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/hlstd.hxx"
#include "openHL/core/hal/interface.h"

namespace hl
{
namespace hal
{

int normHamming(const uchar* a, int n);
int normHamming(const uchar* a, const uchar* b, int n);

int normHamming(const uchar* a, int n, int cellSize);
int normHamming(const uchar* a, const uchar* b, int n, int cellSize);

int  LU32f(float* A, size_t astep, int m, float* b, size_t bstep, int n);
int  LU64f(double* A, size_t astep, int m, double* b, size_t bstep, int n);
bool Cholesky32f(float* A, size_t astep, int m, float* b, size_t bstep, int n);
bool Cholesky64f(double* A, size_t astep, int m, double* b, size_t bstep, int n);

int   normL1_(const uchar* a, const uchar* b, int n);
float normL1_(const float* a, const float* b, int n);
float normL2Sqr_(const float* a, const float* b, int n);

void split8u(const uchar* src, uchar** dst, int len, int cn);
void split16u(const ushort* src, ushort** dst, int len, int cn);
void split32s(const int* src, int** dst, int len, int cn);
void split64s(const int64* src, int64** dst, int len, int cn);

}    // namespace hal
}    // namespace hl