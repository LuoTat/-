#include "precomp.hxx"
#include <queue>

namespace hl
{

static bool isHomogeneous(const Mat& src, int x, int y, int width, int height, int threshold)
{
    int sum   = 0;
    int count = 0;
    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            sum += src.at<uchar>(i, j);
            ++count;
        }
    }
    int mean = sum / count;
    for (int i = y; i < y + height; ++i)
    {
        for (int j = x; j < x + width; ++j)
        {
            if (std::abs(src.at<uchar>(i, j) - mean) > threshold)
            {
                return false;
            }
        }
    }
    return true;
}

static void split(const Mat& src, Mat& dst, int x, int y, int width, int height, int threshold)
{
    if (isHomogeneous(src, x, y, width, height, threshold))
    {
        for (int i = y + 1; i < y + height - 1; ++i)
        {
            for (int j = x + 1; j < x + width - 1; ++j)
            {
                dst.at<uchar>(i, j) = src.at<uchar>(i, j);
            }
        }
    }
    else
    {
        int halfWidth  = width / 2;
        int halfHeight = height / 2;
        if (halfWidth > 0 && halfHeight > 0)
        {
            split(src, dst, x, y, halfWidth, halfHeight, threshold);
            split(src, dst, x + halfWidth, y, halfWidth, halfHeight, threshold);
            split(src, dst, x, y + halfHeight, halfWidth, halfHeight, threshold);
            split(src, dst, x + halfWidth, y + halfHeight, halfWidth, halfHeight, threshold);
        }
    }
}

void regionSplitting(const Mat& src, Mat& dst, int threshold)
{
    // 创建目标图像并初始化为0
    dst.create(src.rows, src.cols, src.type());
    for (int y = 0; y < dst.rows; ++y)
    {
        for (int x = 0; x < dst.cols; ++x)
        {
            dst.at<uchar>(y, x) = 0;
        }
    }

    // 开始区域分裂
    split(src, dst, 0, 0, src.cols, src.rows, threshold);
}

void regionGrowing(const Mat& src, Mat& dst, int seedX, int seedY, int threshold)
{
    // 创建目标图像并初始化为0
    dst.create(src.rows, src.cols, src.type());
    for (int y = 0; y < dst.rows; ++y)
    {
        for (int x = 0; x < dst.cols; ++x)
        {
            dst.at<uchar>(y, x) = 0;
        }
    }

    // 定义方向数组，用于遍历8邻域
    const int dx[8] = {-1, 1, 0, 0, -1, -1, 1, 1};
    const int dy[8] = {0, 0, -1, 1, -1, 1, -1, 1};

    // 使用队列进行广度优先搜索
    std::queue<std::pair<int, int>> q;
    q.push({seedX, seedY});
    dst.at<uchar>(seedY, seedX) = 255;    // 标记种子点

    while (!q.empty())
    {
        int x = q.front().first;
        int y = q.front().second;
        q.pop();

        // 遍历8邻域
        for (int i = 0; i < 8; ++i)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // 检查邻域像素是否在图像范围内
            if (nx >= 0 && nx < src.cols && ny >= 0 && ny < src.rows)
            {
                // 检查邻域像素是否已经被标记
                if (dst.at<uchar>(ny, nx) == 0)
                {
                    // 计算当前像素与种子点的灰度差
                    uchar diff = std::abs(src.at<uchar>(ny, nx) - src.at<uchar>(seedY, seedX));
                    if (diff <= threshold)
                    {
                        // 标记并加入队列
                        dst.at<uchar>(ny, nx) = 255;
                        q.push({nx, ny});
                    }
                }
            }
        }
    }
}

}    // namespace hl