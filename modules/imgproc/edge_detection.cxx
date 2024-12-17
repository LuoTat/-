#include "precomp.hxx"

namespace hl
{

inline static uchar getG(short gx, short gy)
{
    double g = std::sqrt(gx * gx + gy * gy);
    return static_cast<uchar>(g > 255 ? 255 : g);
}

void prewitt(const Mat& src, Mat& dst)
{
    short gx, gy;
    uchar z1, z2, z3, z4, z6, z7, z8, z9;

    // 初始化目标图像，确保尺寸和类型一致
    dst = Mat(src.size(), src.type());

    // 遍历图像的每个像素
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            // 获取3x3邻域像素值，使用最近边缘像素填充防止越界
            z1                  = src.at<uchar>(std::max(y - 1, 0), std::max(x - 1, 0));
            z2                  = src.at<uchar>(std::max(y - 1, 0), x);
            z3                  = src.at<uchar>(std::max(y - 1, 0), std::min(x + 1, src.cols - 1));
            z4                  = src.at<uchar>(y, std::max(x - 1, 0));
            z6                  = src.at<uchar>(y, std::min(x + 1, src.cols - 1));
            z7                  = src.at<uchar>(std::min(y + 1, src.rows - 1), std::max(x - 1, 0));
            z8                  = src.at<uchar>(std::min(y + 1, src.rows - 1), x);
            z9                  = src.at<uchar>(std::min(y + 1, src.rows - 1), std::min(x + 1, src.cols - 1));

            // 计算水平方向梯度
            gx                  = z7 + z8 + z9 - z1 - z2 - z3;
            // 计算垂直方向梯度
            gy                  = z3 + z6 + z9 - z1 - z4 - z7;

            // 设置目标像素值
            dst.at<uchar>(y, x) = getG(gx, gy);
        }
    }
}

void sobel(const Mat& src, Mat& dst)
{
    short gx, gy;
    uchar z1, z2, z3, z4, z6, z7, z8, z9;

    // 初始化目标图像，确保尺寸和类型一致
    dst = Mat(src.size(), src.type());

    // 遍历图像的每个像素
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            // 获取3x3邻域像素值，使用最近边缘像素填充防止越界
            z1                  = src.at<uchar>(std::max(y - 1, 0), std::max(x - 1, 0));
            z2                  = src.at<uchar>(std::max(y - 1, 0), x);
            z3                  = src.at<uchar>(std::max(y - 1, 0), std::min(x + 1, src.cols - 1));
            z4                  = src.at<uchar>(y, std::max(x - 1, 0));
            z6                  = src.at<uchar>(y, std::min(x + 1, src.cols - 1));
            z7                  = src.at<uchar>(std::min(y + 1, src.rows - 1), std::max(x - 1, 0));
            z8                  = src.at<uchar>(std::min(y + 1, src.rows - 1), x);
            z9                  = src.at<uchar>(std::min(y + 1, src.rows - 1), std::min(x + 1, src.cols - 1));

            // 计算水平方向梯度
            gx                  = z7 + 2 * z8 + z9 - z1 - 2 * z2 - z3;
            // 计算垂直方向梯度
            gy                  = z3 + 2 * z6 + z9 - z1 - 2 * z4 - z7;

            // 设置目标像素值
            dst.at<uchar>(y, x) = getG(gx, gy);
        }
    }
}

void LOG(const Mat& src, Mat& dst)
{
    short pixi;
    uchar z3, z7, z8, z9, z11, z12, z13, z14, z15, z17, z18, z19, z23;

    // 初始化目标图像，确保尺寸和类型一致
    dst = Mat(src.size(), src.type());

    // 遍历图像的每个像素
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            // 获取5x5邻域像素值，使用最近边缘像素填充防止越界
            z3                  = src.at<uchar>(std::max(y - 2, 0), x);

            z7                  = src.at<uchar>(std::max(y - 1, 0), std::max(x - 1, 0));
            z8                  = src.at<uchar>(std::max(y - 1, 0), x);
            z9                  = src.at<uchar>(std::max(y - 1, 0), std::min(x + 1, src.cols - 1));

            z11                 = src.at<uchar>(y, std::max(x - 2, 0));
            z12                 = src.at<uchar>(y, std::max(x - 1, 0));
            z13                 = src.at<uchar>(y, x);
            z14                 = src.at<uchar>(y, std::min(x + 1, src.cols - 1));
            z15                 = src.at<uchar>(y, std::min(x + 2, src.cols - 1));

            z17                 = src.at<uchar>(std::min(y + 1, src.rows - 1), std::max(x - 1, 0));
            z18                 = src.at<uchar>(std::min(y + 1, src.rows - 1), x);
            z19                 = src.at<uchar>(std::min(y + 1, src.rows - 1), std::min(x + 1, src.cols - 1));

            z23                 = src.at<uchar>(std::min(y + 2, src.rows - 1), x);

            // 设置目标像素值
            pixi                = -z3 - z7 - 2 * z8 - z9 - z11 - 2 * z12 + 16 * z13 - 2 * z14 - z15 - z17 - 2 * z18 - z19 - z23;
            dst.at<uchar>(y, x) = static_cast<uchar>(pixi > 255 ? 255 : pixi < 0 ? 0
                                                                                 : pixi);
        }
    }
}

}    // namespace hl