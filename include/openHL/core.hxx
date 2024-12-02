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

// 归一化
void normalize(const Mat& src, Mat& dst, double alpha = 1, double beta = 0, int norm_type = NORM_L2, int dtype = -1, const Mat& mask = Mat());

// 计算最大最小值
void minMaxIdx(const Mat& src, double* minVal, double* maxVal = 0, int* minIdx = 0, int* maxIdx = 0, const Mat& mask = Mat());

// 分离通道
void split(const Mat& src, Mat* mvbegin);

}    // namespace hl