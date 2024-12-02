#include "precomp.hxx"
#include <cmath>

namespace hl{

void translate(const Mat& src, Mat& dst, int tx, int ty) {
    // 创建目标图像
    dst.create(src.rows, src.cols, src.type());

    // 计算平移矩阵
    Matrix<double> translationMatrix = {
        {1, 0, static_cast<double>(tx)},
        {0, 1, static_cast<double>(ty)},
        {0, 0, 1}
    };

    // 遍历目标图像的每个像素
    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            // 目标图像中的位置
            Matrix<double> pointMatrix = {
                {static_cast<double>(x)},
                {static_cast<double>(y)},
                {1}
            };

            // 计算源图像中的对应位置
            Matrix<double> srcPointMatrix = multiply(translationMatrix, pointMatrix);
            double srcX = srcPointMatrix[0][0];
            double srcY = srcPointMatrix[1][0];

            // 检查源位置是否在源图像的范围内
            if (srcX >= 0 && srcX < src.cols && srcY >= 0 && srcY < src.rows) {
                // 将源图像的像素值复制到目标图像的新位置
                dst.at<uchar>(y, x) = src.at<uchar>(static_cast<int>(srcY), static_cast<int>(srcX));
            } else {
                // 如果源位置不在源图像范围内，设置为黑色
                dst.at<uchar>(y, x) = 0;
            }
        }
    }
}

void resize(const Mat& src, Mat& dst, int newWidth, int newHeight) {
    // 创建目标图像
    dst.create(newHeight, newWidth, src.type());

    // 计算缩放比例
    double scaleX = static_cast<double>(newWidth) / src.cols;
    double scaleY = static_cast<double>(newHeight) / src.rows;

    // 计算缩放矩阵
    Matrix<double> scaleMatrix = {
        {scaleX, 0, 0},
        {0, scaleY, 0},
        {0, 0, 1}
    };

    // 遍历目标图像的每个像素
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            // 目标图像中的位置
            Matrix<double> pointMatrix = {
                {static_cast<double>(x)},
                {static_cast<double>(y)},
                {1}
            };

            // 计算源图像中的对应位置
            Matrix<double> srcPointMatrix = multiply(inverse(scaleMatrix), pointMatrix);
            double srcX = srcPointMatrix[0][0];
            double srcY = srcPointMatrix[1][0];

            // 获取源图像中的四个邻近像素
            int x1 = static_cast<int>(srcX);
            int y1 = static_cast<int>(srcY);
            int x2 = std::min(x1 + 1, src.cols - 1);
            int y2 = std::min(y1 + 1, src.rows - 1);

            // 计算插值权重
            double dx = srcX - x1;
            double dy = srcY - y1;

            // 获取四个邻近像素的值
            uchar p1 = src.at<uchar>(y1, x1);
            uchar p2 = src.at<uchar>(y1, x2);
            uchar p3 = src.at<uchar>(y2, x1);
            uchar p4 = src.at<uchar>(y2, x2);

            // 计算双线性插值
            uchar value = static_cast<uchar>(
                p1 * (1 - dx) * (1 - dy) +
                p2 * dx * (1 - dy) +
                p3 * (1 - dx) * dy +
                p4 * dx * dy
            );

            // 将插值后的值赋给目标图像
            dst.at<uchar>(y, x) = value;
        }
    }
}

void flip(const Mat& src, Mat& dst, FlipType flipType) {
    // 创建目标图像
    dst.create(src.rows, src.cols, src.type());

    // 计算翻转矩阵
    Matrix<double> flipMatrix(3, 3);
    if (flipType == HORIZONTAL) {
        flipMatrix = {
            {-1, 0, static_cast<double>(src.cols - 1)},
            {0, 1, 0},
            {0, 0, 1}
        };
    } else if (flipType == VERTICAL) {
        flipMatrix = {
            {1, 0, 0},
            {0, -1, static_cast<double>(src.rows - 1)},
            {0, 0, 1}
        };
    }

    // 遍历目标图像的每个像素
    for (int y = 0; y < dst.rows; ++y) {
        for (int x = 0; x < dst.cols; ++x) {
            // 目标图像中的位置
            Matrix<double> pointMatrix = {
                {static_cast<double>(x)},
                {static_cast<double>(y)},
                {1}
            };

            // 计算源图像中的对应位置
            Matrix<double> srcPointMatrix = multiply(flipMatrix, pointMatrix);
            double srcX = srcPointMatrix[0][0];
            double srcY = srcPointMatrix[1][0];

            // 检查源位置是否在源图像的范围内
            if (srcX >= 0 && srcX < src.cols && srcY >= 0 && srcY < src.rows) {
                // 将源图像的像素值复制到目标图像的新位置
                dst.at<uchar>(y, x) = src.at<uchar>(static_cast<int>(srcY), static_cast<int>(srcX));
            } else {
                // 如果源位置不在源图像范围内，设置为黑色
                dst.at<uchar>(y, x) = 0;
            }
        }
    }
}

void rotate(const Mat& src, Mat& dst, double angle) {
    // 计算旋转角度的弧度值
    double radians = angle * PI / 180.0;

    // 计算旋转矩阵
    Matrix<double> rotationMatrix = {
        {std::cos(radians), std::sin(radians), 0},
        {-std::sin(radians), std::cos(radians), 0},
        {0, 0, 1}
    };

    // 计算目标图像的尺寸
    int newWidth = static_cast<int>(std::abs(src.cols * std::cos(radians)) + std::abs(src.rows * std::sin(radians)));
    int newHeight = static_cast<int>(std::abs(src.cols * std::sin(radians)) + std::abs(src.rows * std::cos(radians)));

    // 创建目标图像
    dst.create(newHeight, newWidth, src.type());

    // 计算图像中心
    int cx = src.cols / 2;
    int cy = src.rows / 2;
    int newCx = newWidth / 2;
    int newCy = newHeight / 2;

    // 遍历目标图像的每个像素
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            // 目标图像中的位置
            Matrix<double> pointMatrix = {
                {static_cast<double>(x - newCx)},
                {static_cast<double>(y - newCy)},
                {1}
            };

            // 计算源图像中的对应位置
            Matrix<double> srcPointMatrix = multiply(inverse(rotationMatrix), pointMatrix);
            double srcX = srcPointMatrix[0][0] + cx;
            double srcY = srcPointMatrix[1][0] + cy;

            // 检查源位置是否在源图像的范围内
            if (srcX >= 0 && srcX < src.cols && srcY >= 0 && srcY < src.rows) {
                // 将源图像的像素值复制到目标图像的新位置
                dst.at<uchar>(y, x) = src.at<uchar>(static_cast<int>(srcY), static_cast<int>(srcX));
            } else {
                // 如果源位置不在源图像范围内，设置为黑色
                dst.at<uchar>(y, x) = 0;
            }
        }
    }
}

}