#include "precomp.hxx"

namespace hl
{

static bool isInnerPoint(const Mat& src, int y, int x)
{
    // 定义方向数组，用于遍历8邻域
    const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

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
            if (src.at<uchar>(ny, nx) == 255)
            {
                // 返回下一个边界点
                p[0] = ny;
                p[1] = nx;
                if (p[2] & 1)                  // 如果是奇数方向
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

    // 寻找起始点
    Vec3i startPoint;
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            if (src.at<uchar>(y, x) == 255)
            {
                startPoint = Vec3i(y, x, 5);
                break;
            }
        }
    }

    Vec3i p = startPoint;
    // 搜索轮廓

    do
    {
        dst.at<Vec3b>(p[0], p[1]) = Vec3b(0, 0, 255);
        nextPoint(src, p);
    }
    while (p != Vec3i(-1, -1, -1) && p != startPoint);

    // // // 创建一个标记矩阵，用于标记已经访问过的像素
    // // Mat visited(src.rows, src.cols, src.type());
    // // for (int y = 0; y < visited.rows; ++y)
    // // {
    // //     for (int x = 0; x < visited.cols; ++x)
    // //     {
    // //         visited.at<uchar>(y, x) = 0;
    // //     }
    // // }



    // // 遍历图像中的每个像素
    // for (int y = 0; y < src.rows; ++y)
    // {
    //     for (int x = 0; x < src.cols; ++x)
    //     {
    //         if (src.at<uchar>(y, x) == 0 && visited.at<uchar>(y, x) == 0 && isExternalPoint(src, x, y))
    //         {    // 只处理边缘外部点
    //             std::vector<Point> contour;
    //             std::queue<Point>  q;
    //             q.push(Point(x, y));
    //             visited.at<uchar>(y, x) = 1;

    //             while (!q.empty())
    //             {
    //                 Point p = q.front();
    //                 q.pop();
    //                 contour.push_back(p);

    //                 int dir = p.dir % 2 ? (p.dir + 6) % 8 : (p.dir + 7) % 8;

    //                 // 遍历8邻域
    //                 for (int i = 0; i < 8; ++i)
    //                 {
    //                     int nx = p.x + dx[(dir + i) % 8];
    //                     int ny = p.y + dy[(dir + i) % 8];

    //                     // 检查邻域像素是否在图像范围内
    //                     if (nx >= 0 && nx < src.cols && ny >= 0 && ny < src.rows)
    //                     {
    //                         if (nx == contour[0].x && ny == contour[0].y) break;
    //                         // 检查邻域像素是否已经被访问且是外部点
    //                         if (src.at<uchar>(ny, nx) == 0 && visited.at<uchar>(ny, nx) == 0 && isExternalPoint(src, nx, ny))
    //                         {
    //                             visited.at<uchar>(ny, nx) = 1;
    //                             q.push(Point(nx, ny, (dir + i) % 8));
    //                         }
    //                     }
    //                 }
    //             }

    //             contours.push_back(contour);
    //         }
    //     }
    // }
}

}    // namespace hl