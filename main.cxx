#include "openHL/openHL.hxx"
#include <iostream>

using namespace hl;

// 功能菜单
static void showMenu()
{
    std::cout << "\n===========================\n";
    std::cout << "        图像处理工具        \n";
    std::cout << "===========================\n";
    std::cout << "请选择功能：\n";
    std::cout << "1. BMP文件处理\n";
    std::cout << "2. 直方图处理\n";
    std::cout << "3. 空间滤波\n";
    std::cout << "4. 图像变换\n";
    std::cout << "5. 阈值分割\n";
    std::cout << "6. 区域分割\n";
    std::cout << "7. 边缘检测\n";
    std::cout << "8. Hough变换\n";
    std::cout << "9. 区域标记\n";
    std::cout << "10. 轮廓提取\n";
    std::cout << "0. 退出\n";
    std::cout << "===========================\n";
    std::cout << "请输入数字来运行功能：";
}

int main()
{
    ushort choice;

    do {
        showMenu();
        std::cin >> choice;

        if (choice == 1)
        {
            Mat rgb = imread("../Test/rgb.bmp", IMREAD_UNCHANGED);
            Mat rgb_gray, rgb_gray_reverse, splited_array[3];

            // 转换为灰度图像
            cvtColor(rgb, rgb_gray, COLOR_BGR2GRAY);
            imwrite("../Test/rgb_gray.bmp", rgb_gray);

            // 灰度反转
            Mat temp = Mat(rgb_gray.size(), rgb_gray.type(), Scalar(255));
            subtract(temp, rgb_gray, rgb_gray_reverse);
            imwrite("../Test/rgb_gray_reverse.bmp", rgb_gray_reverse);

            // 分离通道
            split(rgb, splited_array);
            imwrite("../Test/rgb_B.bmp", splited_array[0]);
            imwrite("../Test/rgb_G.bmp", splited_array[1]);
            imwrite("../Test/rgb_R.bmp", splited_array[2]);
        }
        else if (choice == 2)
        {
            Mat dim = imread("../Test/dim.bmp", IMREAD_GRAYSCALE);
            Mat dim_equalized, dim_equalized_hist, dim_hist, dim_hist_img, dim_equalized_hist_img;

            // 直方图均衡化
            equalizeHist(dim, dim_equalized);

            int          channels[] = {0};
            int          histSize[] = {256};
            float        range[]    = {0, 256};
            const float* ranges[]   = {range};

            calcHist(&dim, 1, channels, Mat(), dim_hist, 1, histSize, ranges);
            calcHist(&dim_equalized, 1, channels, Mat(), dim_equalized_hist, 1, histSize, ranges);
            drawHist(dim_hist, dim_hist_img, 4, 1024);
            drawHist(dim_equalized_hist, dim_equalized_hist_img, 4, 1024);

            imwrite("../Test/dim_hist_img.bmp", dim_hist_img);
            imwrite("../Test/dim_equalized_hist_img.bmp", dim_equalized_hist_img);
            imwrite("../Test/dim_equalized.bmp", dim_equalized);
        }
        else if (choice == 3)
        {
            Mat noise = imread("../Test/noise.bmp", IMREAD_GRAYSCALE);
            Mat noise_medianblur, noise_blur;

            // 中值滤波
            medianBlur(noise, noise_medianblur, 5);

            // 均值滤波
            blur(noise, noise_blur, Size(5, 5));

            imwrite("../Test/noise_medianblur.bmp", noise_medianblur);
            imwrite("../Test/noise_blur.bmp", noise_blur);
        }
        else if (choice == 4)
        {
            Mat lena = imread("../Test/lena.bmp", IMREAD_GRAYSCALE);
            Mat lena_resize, lena_shift, lena_flip, lena_rotate;

            // 缩放
            resize(lena, lena_resize, Size(4096, 4096), INTER_LANCZOS4);
            imwrite("../Test/lena_resize.bmp", lena_resize);

            // 平移
            Mat shift = (Mat)Matx<float, 2, 3>(1, 0, 100, 0, 1, 100);
            warpAffine(lena, lena_shift, shift, lena.size());
            imwrite("../Test/lena_shift.bmp", lena_shift);

            // 翻转
            flip(lena, lena_flip, 1);
            imwrite("../Test/lena_flip.bmp", lena_flip);

            // 旋转
            Mat rotate = getRotationMatrix2D(Point(lena.cols / 2, lena.rows / 2), 45, 1);
            warpAffine(lena, lena_rotate, rotate, lena.size());
            imwrite("../Test/lena_rotate.bmp", lena_rotate);
        }
        else if (choice == 5)
        {
            Mat lena = imread("../Test/lena.bmp", IMREAD_GRAYSCALE);

            Mat          lena_hist;
            int          channels[] = {0};
            int          histSize[] = {256};
            float        range[]    = {0, 256};
            const float* ranges[]   = {range};

            calcHist(&lena, 1, channels, Mat(), lena_hist, 1, histSize, ranges);

            Mat lena_threshold_T, lena_threshold_Iter, lena_threshold_Otsu;
            Mat lena_threshold_T_hist_img, lena_threshold_Iter_hist_img, lena_threshold_Otsu_hist_img;

            // 给定阈值分割
            drawHist_T(lena_hist, lena_threshold_T_hist_img, 4, 1024, static_cast<uchar>(threshold(lena, lena_threshold_T, 80, 255, THRESH_BINARY)));

            // 迭代阈值分割
            // drawHist_T(lena_hist, lena_threshold_Iter_hist_img, 4, 1024, static_cast<uchar>(threshold(lena, lena_threshold_Iter, 114, 255, THRESH_BINARY)));

            // Otsu阈值分割
            drawHist_T(lena_hist, lena_threshold_Otsu_hist_img, 4, 1024, static_cast<uchar>(threshold(lena, lena_threshold_Otsu, 0, 255, THRESH_OTSU)));

            imwrite("../Test/lena_threshold_T.bmp", lena_threshold_T);
            // imwrite("../Test/lena_threshold_Iter.bmp", lena_threshold_Iter);
            imwrite("../Test/lena_threshold_Otsu.bmp", lena_threshold_Otsu);

            imwrite("../Test/lena_threshold_T_hist.bmp", lena_threshold_T_hist_img);
            // imwrite("../Test/lena_threshold_Iter_hist.bmp", lena_threshold_Iter_hist_img);
            imwrite("../Test/lena_threshold_Otsu_hist.bmp", lena_threshold_Otsu_hist_img);

            std::cout << "已进行阈值分割.\n";
        }
        else if (choice == 6)
        {
        }
        else if (choice == 7)
        {
            Mat lena = imread("../Test/lena.bmp", IMREAD_GRAYSCALE);
            Mat lena_prewitt, lena_sobel, lena_LOG;

            // Prewitt算子
            prewitt(lena, lena_prewitt);
            threshold(lena_prewitt, lena_prewitt, 0, 255, THRESH_OTSU);
            imwrite("../Test/lena_prewitt.bmp", lena_prewitt);

            // Sobel算子
            sobel(lena, lena_sobel);
            threshold(lena_sobel, lena_sobel, 0, 255, THRESH_OTSU);
            imwrite("../Test/lena_sobel.bmp", lena_sobel);

            // LOG算子
            LOG(lena, lena_LOG);
            threshold(lena_LOG, lena_LOG, 0, 255, THRESH_OTSU);
            imwrite("../Test/lena_LOG.bmp", lena_LOG);
        }
        else if (choice == 8)
        {
        }
        else if (choice == 9)
        {
        }
        else if (choice == 10)
        {
        }
        else if (choice == 0)
            std::cout << "已退出。\n";
        else std::cout << "输入错误，请重新输入。\n";
    }
    while (choice != 0);

    return 0;
}
