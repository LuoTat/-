#include "openHL/openHL.hxx"
#include <iostream>

using namespace hl;

// 功能菜单
void showMenu()
{
    std::cout << "\n===========================\n";
    std::cout << "        图像处理工具        \n";
    std::cout << "===========================\n";
    std::cout << "请选择功能：\n";
    std::cout << "1. 将图像转换为灰度图\n";
    // std::cout << "2. 将图像转换为反转灰度图\n";
    std::cout << "3. 分离图像通道\n";
    std::cout << "4. 直方图处理\n";
    std::cout << "5. 图像平滑处理\n";
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
            // 转换为灰度图像
            Mat rgb, rgb_gray;
            rgb = imread("../rgb.bmp", IMREAD_UNCHANGED);
            cvtColor(rgb, rgb_gray, COLOR_BGR2GRAY);
            imwrite("../rgb_gray.bmp", rgb_gray);
            std::cout << "已将图像转换为灰度图，保存为 rgb_gray.bmp\n";
        }
        // else if (choice == 2)
        // {
        //     // 转换为反转灰度图像
        //     Mat rgb, rgb_gray_inverted;
        //     rgb = imread("rgb.bmp");
        //     cvtColor(&rgb, &rgb_gray_inverted, COLOR_BGR2GRAY_INVERTED);
        //     imwrite("rgb_gray_inverted.bmp", &rgb_gray_inverted);
        //     printf("已将图像转换为反转灰度图，保存为 rgb_gray_inverted.bmp\n");
        //     release(&rgb);
        //     release(&rgb_gray_inverted);
        // }
        else if (choice == 3)
        {
            // 分离通道
            Mat rgb;
            Mat splited_array[3];
            rgb = imread("../rgb.bmp", IMREAD_UNCHANGED);
            split(rgb, splited_array);
            imwrite("../rgb_B.bmp", splited_array[0]);
            imwrite("../rgb_G.bmp", splited_array[1]);
            imwrite("../rgb_R.bmp", splited_array[2]);
            std::cout << "已分离图像通道，分别保存为 rgb_B.bmp, rgb_G.bmp, rgb_R.bmp\n";
        }
        else if (choice == 4)
        {
            // 直方图处理
            Mat dim, dim_equalized, dim_equalized_hist, dim_hist, dim_hist_img, dim_equalized_hist_img;

            dim = imread("../dim.bmp", IMREAD_GRAYSCALE);
            equalizeHist(dim, dim_equalized);

            int          channels[] = {0};
            int          histSize[] = {256};
            float        range[]    = {0, 256};
            const float* ranges[]   = {range};

            calcHist(&dim, 1, channels, Mat(), dim_hist, 1, histSize, ranges);
            calcHist(&dim_equalized, 1, channels, Mat(), dim_equalized_hist, 1, histSize, ranges);
            drawHist(dim_hist, dim_hist_img, 4, 1024);
            drawHist(dim_equalized_hist, dim_equalized_hist_img, 4, 1024);
            imwrite("../dim_hist_img.bmp", dim_hist_img);
            imwrite("../dim_equalized_hist_img.bmp", dim_equalized_hist_img);
            imwrite("../dim_equalized.bmp", dim_equalized);
            std::cout << "已生成直方图均衡化图像，保存为 dim_equalized.bmp\n";
        }
        else if (choice == 5)
        {
            Mat noise = imread("../noise.bmp", IMREAD_GRAYSCALE);
            Mat noise_medianblur, noise_blur;

            medianBlur(noise, noise_medianblur, 5);
            blur(noise, noise_blur, Size(5, 5));
            imwrite("../noise_medianblur.bmp", noise_medianblur);
            imwrite("../noise_blur.bmp", noise_blur);
        }
        else if (choice == 0)
            std::cout
                << "已退出。\n";
        else std::cout << "输入错误，请重新输入。\n";
    }
    while (choice != 0);

    return 0;
}
