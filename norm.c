#include "base.h"
#include "core.h"
#include "mat.h"
#include <float.h>
#include <stdio.h>

void normalize(const Mat* src, Mat* dst, int min, int max, enum NormTypes norm_type)
{
    double scale = 1;    // 缩放比例
    double shift = 0;    // 偏移系数
    if (norm_type == NORM_MINMAX)
    {
        double smin = 0, smax = 0;

        //获取输入数组 src 的最小值 smin 和最大值 smax
        minMaxIdx(src, &smin, &smax, NULL, NULL);

        // 计算缩放比例和偏移系数
        scale = (max - min) * (smax - smin > DBL_EPSILON ? 1.0 / (smax - smin) : 0);
        shift = min - smin * scale;
    }
    else if (norm_type == NORM_L1 || norm_type == NORM_L2 || norm_type == NORM_INF)
    {}
    else
    {
        printf("norm_type error\n");
        return;
    }
    convertTo(src, dst, scale, shift);
}