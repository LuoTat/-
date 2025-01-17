#pragma once

#include "openHL/core/hldef.h"
#include "openHL/core/base.hxx"
#include "openHL/core/hlstd.hxx"
#include "openHL/core/traits.hxx"
#include "openHL/core/matx.hxx"
#include "openHL/core/types.hxx"
#include "openHL/core/mat.hxx"

namespace hl
{

class Exception: public std::exception
{
public:
    Exception();
    Exception(int _code, const String& _err, const String& _func, const String& _file, int _line);
    virtual ~Exception() throw();

    virtual const char* what() const throw() override;
    void                formatMessage();

    String msg;
    int    code;
    String err;
    String func;
    String file;
    int    line;
};

[[noreturn]]
void error(const Exception& exc);

int borderInterpolate(int p, int len, int borderType);

void copyMakeBorder(const Mat& src, Mat& dst, int top, int bottom, int left, int right, int borderType, const Scalar& value = Scalar());

void add(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask = Mat(), int dtype = -1);

void subtract(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask = Mat(), int dtype = -1);

double norm(const Mat& src1, int normType = NORM_L2, const Mat& mask = Mat());

double norm(const Mat& src1, const Mat& src2, int normType = NORM_L2, const Mat& mask = Mat());

void normalize(const Mat& src, Mat& dst, double alpha = 1, double beta = 0, int norm_type = NORM_L2, int dtype = -1, const Mat& mask = Mat());

void minMaxIdx(const Mat& src, double* minVal, double* maxVal = 0, int* minIdx = 0, int* maxIdx = 0, const Mat& mask = Mat());

void split(const Mat& src, Mat* mvbegin);

void flip(const Mat& src, Mat& dst, int flipCode);

void bitwise_and(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask = Mat());

void bitwise_or(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask = Mat());

void bitwise_xor(const Mat& src1, const Mat& src2, Mat& dst, const Mat& mask = Mat());

void bitwise_not(const Mat& src, Mat& dst, const Mat& mask = Mat());

void absdiff(const Mat& src1, const Mat& src2, Mat& dst);

void copyTo(const Mat& src, Mat& dst, const Mat& mask);

void min(const Mat& src1, const Mat& src2, Mat& dst);

void max(const Mat& src1, const Mat& src2, Mat& dst);

}    // namespace hl