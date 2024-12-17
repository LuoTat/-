#include "precomp.hxx"

namespace hl
{

static void drawLine(Mat& img, int rho, int theta)
{
    // 将 theta 从度转换为弧度
    double thetaRad = theta * HL_PI / 180.0;

    // 计算 cos(theta) 和 sin(theta)
    double cosTheta = std::abs(std::cos(thetaRad)) < DBL_EPSILON ? 0 : std::cos(thetaRad);
    double sinTheta = std::abs(std::sin(thetaRad)) < DBL_EPSILON ? 0 : std::sin(thetaRad);

    // 图像的宽度和高度
    int width       = img.cols;
    int height      = img.rows;

    // 存储四个交点
    Point2d pt1, pt2, pt3, pt4;

    // 1. 计算直线与上下边界的交点
    pt1 = Point2d(rho / cosTheta, 0);                                         // 上边界 y = 0
    pt2 = Point2d((rho - (height - 1) * sinTheta) / cosTheta, height - 1);    // 下边界 y = height - 1

    // 2. 计算直线与左右边界的交点
    pt3 = Point2d(0, rho / sinTheta);                                       // 左边界 x = 0
    pt4 = Point2d(width - 1, (rho - (width - 1) * cosTheta) / sinTheta);    // 右边界 x = width - 1

    // 3. 筛选有效点
    std::vector<Point2d> validPoints;
    if (pt1.x >= 0 && pt1.x < width) validPoints.push_back(pt1);     // 上边界
    if (pt2.x >= 0 && pt2.x < width) validPoints.push_back(pt2);     // 下边界
    if (pt3.y >= 0 && pt3.y < height) validPoints.push_back(pt3);    // 左边界
    if (pt4.y >= 0 && pt4.y < height) validPoints.push_back(pt4);    // 右边界

    // 4. 确保至少有两个点可以绘制
    if (validPoints.size() < 2)
        return;

    Point p1       = validPoints[0];
    Point p2       = validPoints[1];

    // 手动绘制线段，使用 Bresenham 直线算法
    auto drawPixel = [&](int x, int y)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            img.at<Vec3b>(y, x) = Vec3b(0, 0, 255);    // 红色像素
        }
    };

    // Bresenham 直线绘制
    int x1 = p1.x, y1 = p1.y, x2 = p2.x, y2 = p2.y;
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx  = (x1 < x2) ? 1 : -1;
    int sy  = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        drawPixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1  += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1  += sy;
        }
    }
}

void HoughLines(const Mat& src, Mat& dst, uint threshold)
{
    // 初始化霍夫空间
    int thetaMin = -90, thetaMax = 180;
    int rhoBins     = std::floor(std::sqrt(src.cols * src.cols + src.rows * src.rows));

    // 创建霍夫空间
    Mat accumulator = Mat(rhoBins, thetaMax - thetaMin, HL_32SC1, Scalar(0));

    // 遍历图像中的每个像素
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            if (src.at<uchar>(y, x) == 255)
            {    // 只处理边缘点
                for (int theta = thetaMin + 1; theta < thetaMax; ++theta)
                {
                    int rho = std::lround(y * sin(theta * HL_PI / 180) + x * cos(theta * HL_PI / 180));
                    if (rho >= 0)
                        ++accumulator.at<uint>(rho, theta - thetaMin);
                }
            }
        }
    }

    // 将src复制到dst
    dst.create(src.rows, src.cols, HL_8UC3);
    for (int y = 0; y < src.rows; ++y)
    {
        for (int x = 0; x < src.cols; ++x)
        {
            dst.at<Vec3b>(y, x) = Vec3b(src.at<uchar>(y, x), src.at<uchar>(y, x), src.at<uchar>(y, x));
        }
    }

    for (int rho = 0; rho < accumulator.rows; ++rho)
    {
        for (int theta = 0; theta < accumulator.cols; ++theta)
        {
            if (accumulator.at<uint>(rho, theta) >= threshold)
            {
                drawLine(dst, rho, theta + thetaMin);    // 画线
            }
        }
    }
}

}    // namespace hl