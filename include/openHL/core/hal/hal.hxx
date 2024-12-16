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

void add8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void add8s(const schar* src1, size_t step1, const schar* src2, size_t step2, schar* dst, size_t step, int width, int height, void*);
void add16u(const ushort* src1, size_t step1, const ushort* src2, size_t step2, ushort* dst, size_t step, int width, int height, void*);
void add16s(const short* src1, size_t step1, const short* src2, size_t step2, short* dst, size_t step, int width, int height, void*);
void add32u(const uint* src1, size_t step1, const uint* src2, size_t step2, uint* dst, size_t step, int width, int height, void*);
void add32s(const int* src1, size_t step1, const int* src2, size_t step2, int* dst, size_t step, int width, int height, void*);
void add32f(const float* src1, size_t step1, const float* src2, size_t step2, float* dst, size_t step, int width, int height, void*);
void add64f(const double* src1, size_t step1, const double* src2, size_t step2, double* dst, size_t step, int width, int height, void*);

void sub8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void sub8s(const schar* src1, size_t step1, const schar* src2, size_t step2, schar* dst, size_t step, int width, int height, void*);
void sub16u(const ushort* src1, size_t step1, const ushort* src2, size_t step2, ushort* dst, size_t step, int width, int height, void*);
void sub16s(const short* src1, size_t step1, const short* src2, size_t step2, short* dst, size_t step, int width, int height, void*);
void sub32u(const uint* src1, size_t step1, const uint* src2, size_t step2, uint* dst, size_t step, int width, int height, void*);
void sub32s(const int* src1, size_t step1, const int* src2, size_t step2, int* dst, size_t step, int width, int height, void*);
void sub32f(const float* src1, size_t step1, const float* src2, size_t step2, float* dst, size_t step, int width, int height, void*);
void sub64f(const double* src1, size_t step1, const double* src2, size_t step2, double* dst, size_t step, int width, int height, void*);

void max8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void max8s(const schar* src1, size_t step1, const schar* src2, size_t step2, schar* dst, size_t step, int width, int height, void*);
void max16u(const ushort* src1, size_t step1, const ushort* src2, size_t step2, ushort* dst, size_t step, int width, int height, void*);
void max16s(const short* src1, size_t step1, const short* src2, size_t step2, short* dst, size_t step, int width, int height, void*);
void max32u(const uint* src1, size_t step1, const uint* src2, size_t step2, uint* dst, size_t step, int width, int height, void*);
void max32s(const int* src1, size_t step1, const int* src2, size_t step2, int* dst, size_t step, int width, int height, void*);
void max32f(const float* src1, size_t step1, const float* src2, size_t step2, float* dst, size_t step, int width, int height, void*);
void max64f(const double* src1, size_t step1, const double* src2, size_t step2, double* dst, size_t step, int width, int height, void*);

void min8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void min8s(const schar* src1, size_t step1, const schar* src2, size_t step2, schar* dst, size_t step, int width, int height, void*);
void min16u(const ushort* src1, size_t step1, const ushort* src2, size_t step2, ushort* dst, size_t step, int width, int height, void*);
void min16s(const short* src1, size_t step1, const short* src2, size_t step2, short* dst, size_t step, int width, int height, void*);
void min32u(const uint* src1, size_t step1, const uint* src2, size_t step2, uint* dst, size_t step, int width, int height, void*);
void min32s(const int* src1, size_t step1, const int* src2, size_t step2, int* dst, size_t step, int width, int height, void*);
void min32f(const float* src1, size_t step1, const float* src2, size_t step2, float* dst, size_t step, int width, int height, void*);
void min64f(const double* src1, size_t step1, const double* src2, size_t step2, double* dst, size_t step, int width, int height, void*);

void absdiff8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void absdiff8s(const schar* src1, size_t step1, const schar* src2, size_t step2, schar* dst, size_t step, int width, int height, void*);
void absdiff16u(const ushort* src1, size_t step1, const ushort* src2, size_t step2, ushort* dst, size_t step, int width, int height, void*);
void absdiff16s(const short* src1, size_t step1, const short* src2, size_t step2, short* dst, size_t step, int width, int height, void*);
void absdiff32u(const uint* src1, size_t step1, const uint* src2, size_t step2, uint* dst, size_t step, int width, int height, void*);
void absdiff32s(const int* src1, size_t step1, const int* src2, size_t step2, int* dst, size_t step, int width, int height, void*);
void absdiff32f(const float* src1, size_t step1, const float* src2, size_t step2, float* dst, size_t step, int width, int height, void*);
void absdiff64f(const double* src1, size_t step1, const double* src2, size_t step2, double* dst, size_t step, int width, int height, void*);

void and8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void or8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void xor8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);
void not8u(const uchar* src1, size_t step1, const uchar* src2, size_t step2, uchar* dst, size_t step, int width, int height, void*);

int  LU(float* A, size_t astep, int m, float* b, size_t bstep, int n);
int  LU(double* A, size_t astep, int m, double* b, size_t bstep, int n);
bool Cholesky(float* A, size_t astep, int m, float* b, size_t bstep, int n);
bool Cholesky(double* A, size_t astep, int m, double* b, size_t bstep, int n);

}    // namespace hal
}    // namespace hl