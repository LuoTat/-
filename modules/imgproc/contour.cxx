#include "openHL/imgcodecs.hxx"
#include "precomp.hxx"
#include <iostream>
#include <unordered_set>

namespace hl
{

struct Vec2iHash
{
    std::size_t operator()(const Vec2i& v) const noexcept
    {
        return std::hash<int>()(v[0]) ^ std::hash<int>()(v[1]);
    }
};

struct Vec2iEqual
{
    bool operator()(const Vec2i& v1, const Vec2i& v2) const noexcept
    {
        return v1[0] == v2[0] && v1[1] == v2[1];
    }
};

static bool isInnerPoint(const Mat& src, int y, int x)
{
    // 定义方向数组，用于遍历8邻域
    const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    if (src.at<uchar>(y, x) == 0) return false;    // 如果该点是背景点，则直接返回false

    // 遍历8邻域
    for (int i = 0; i < 8; ++i)
    {
        int ny = y + dy[i];
        int nx = x + dx[i];

        // 检查邻域像素是否在图像范围内
        if (nx >= 0 && nx < src.cols && ny >= 0 && ny < src.rows)
        {
            // 如果邻域中存在背景点，则该点是边界点
            if (src.at<uchar>(ny, nx) == 0)
            {
                return false;
            }
        }
    }

    // 如果邻域中不存在背景点，则该点是内部点
    return true;
}

void extractContours(const Mat& src, Mat& dst)
{
    cvtColor(src, dst, COLOR_GRAY2BGR);
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            if (src.at<uchar>(y, x) != 255) continue;
            if (isInnerPoint(src, y, x))
            {
                dst.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
            }
            else
            {
                dst.at<Vec3b>(y, x) = Vec3b(0, 0, 255);
            }
        }
    }
}

static void nextPoint(const Mat& src, Vec3i& p)
{
    // 定义方向数组，用于遍历8邻域
    const static int dy[8] = {0, -1, -1, -1, 0, +1, +1, +1};
    const static int dx[8] = {+1, +1, 0, -1, -1, -1, 0, +1};

    for (int i = 0; i < 8; ++i)
    {
        int index = (p[2] + i) & 7;
        int ny    = p[0] + dy[index];
        int nx    = p[1] + dx[index];

        // 检查邻域像素是否在图像范围内
        if (nx >= 0 && nx < src.cols && ny >= 0 && ny < src.rows)
        {
            // if (src.at<uchar>(ny, nx) == 255)
            if (src.at<uchar>(ny, nx) == 255 && !isInnerPoint(src, ny, nx))
            {
                // 返回下一个边界点
                p[0] = ny;
                p[1] = nx;
                if (index & 1)                 // 如果是奇数方向
                {
                    p[2] = (index + 6) & 7;    //顺时针旋转两个方向
                }
                else
                {
                    p[2] = (index + 7) & 7;    //逆时针旋转两个方向
                }
                return;
            }
        }
    }

    // 如果没有找到下一个边界点，则返回(-1, -1, -1)
    p[0] = p[1] = p[2] = -1;
}

void trackingContours(const Mat& src, Mat& dst)
{
    cvtColor(src, dst, COLOR_GRAY2BGR);

    std::unordered_set<Vec2i, Vec2iHash, Vec2iEqual> pointset;

    // 寻找起始点
    Vec3i p;
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            if (src.at<uchar>(y, x) == 255 && !isInnerPoint(src, y, x))
            {
                p = Vec3i(y, x, 5);

                // 搜索轮廓
                do
                {
                    if (pointset.contains(Vec2i(p[0], p[1]))) break;
                    pointset.insert(Vec2i(p[0], p[1]));
                    dst.at<Vec3b>(p[0], p[1]) = Vec3b(0, 0, 255);
                    nextPoint(src, p);
                    if (p == Vec3i(-1, -1, -1)) break;
                    if (p[0] == y && p[1] == x) break;
                }
                while (true);
            }
        }
    }
}

}    // namespace hl