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
    // std::cout << "5. 图像平滑处理\n";
    std::cout << "6. 图像平移\n";
    std::cout << "7. 图像缩放\n";
    std::cout << "8. 镜像反转\n";
    std::cout << "9. 图像旋转\n";
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
            Mat rgb_gray, rgb_hist;
            rgb_gray                = imread("../rgb.bmp", IMREAD_GRAYSCALE);

            int          channels[] = {0};
            Mat          mask       = Mat();
            int          histSize[] = {256};
            float        range[]    = {0, 256};
            const float* ranges[]   = {range};

            calcHist(&rgb_gray, 1, channels, mask, rgb_hist, 1, histSize, ranges);

            for (int i = 0; i < 256; i++)
            {
                std::cout << "[" << i << "] = " << rgb_hist.at<float>(i) << std::endl;
            }


            // Mat dim, dim_hist, dim_hist_norm, dim_hist_norm_img;
            // Mat dim_equalized, dim_equalized_hist, dim_equalized_hist_norm, dim_equalized_hist_norm_img;
            // dim = imread("dim.bmp");
            // calcHist(&dim, &dim_hist);
            // normalize(&dim_hist, &dim_hist_norm, 0, 1000, NORM_MINMAX);
            // drawHist(&dim_hist_norm, &dim_hist_norm_img, 10, 1100);
            // imwrite("dim_hist_norm_img.bmp", &dim_hist_norm_img);
            // printf("已生成灰度直方图，保存为 dim_hist_norm_img.bmp\n");

            // equalizeImage(&dim);
            // imwrite("dim_equalized.bmp", &dim);
            // printf("已生成均衡化图像，保存为 dim_equalized.bmp\n");
            // calcHist(&dim, &dim_equalized_hist);
            // normalize(&dim_equalized_hist, &dim_equalized_hist_norm, 0, 1000, NORM_MINMAX);
            // drawHist(&dim_equalized_hist_norm, &dim_equalized_hist_norm_img, 10, 1100);
            // imwrite("dim_equalized_hist_norm_img.bmp", &dim_equalized_hist_norm_img);
            // printf("已生成直方图均衡化图像，保存为 dim_equalized_hist_norm_img.bmp\n");


            // deleteMat(&dim);
            // deleteMat(&dim_hist);
            // deleteMat(&dim_hist_norm);
            // deleteMat(&dim_hist_norm_img);
            // deleteMat(&dim_equalized);
            // deleteMat(&dim_equalized_hist);
            // deleteMat(&dim_equalized_hist_norm);
            // deleteMat(&dim_equalized_hist_norm_img);
        }
        // else if (choice == 5)
        // {
        //     Mat noise = imread("../noise.bmp", IMREAD_GRAYSCALE);
        //     Mat noise_blur;
        //     Mat noise_median;
        //     blurs(&noise, &noise_blur, 5);
        //     medianBlur(&noise, &noise_median, 5);
        //     imwrite("../noise_blur.bmp", &noise_blur);
        //     imwrite("../noise_median.bmp", &noise_median);
        // }
        else if (choice == 6) {

            Mat mat = imread("../lena.bmp",IMREAD_GRAYSCALE);

            if (mat.empty()) {
                std::cerr << "Error: Image cannot be loaded!" << std::endl;
                return -1;
            }

            // 定义平移参数
            int tx = 50; // x方向平移量
            int ty = 30; // y方向平移量

            // 创建一个新的Mat对象来存储平移后的图像
            Mat translated_mat;
            translate(mat, translated_mat, tx, ty);

            // 保存图像
            imwrite("../translated_image.bmp", translated_mat);
        }
        else if (choice == 7) {
            // 加载图像
            Mat mat = imread("../lena.bmp",IMREAD_GRAYSCALE);

            if (mat.empty()) {
                std::cerr << "Error: Image cannot be loaded!" << std::endl;
                return -1;
            }

            // 定义新的宽度和高度
            int newWidth = 100; // 新的宽度
            int newHeight = 100; // 新的高度

            // 创建一个新的Mat对象来存储缩放后的图像
            Mat resized_mat;
            resize(mat, resized_mat, newWidth, newHeight);

            // 假设我们有一个保存图像的函数
            imwrite("../resized_image.bmp", resized_mat);
        }
        else if (choice == 8) {
            // 加载图像
            Mat mat = imread("../lena.bmp",IMREAD_GRAYSCALE);

            if (mat.empty()) {
                std::cerr << "Error: Image cannot be loaded!" << std::endl;
                return -1;
            }

            // 创建一个新的Mat对象来存储水平镜像后的图像
            Mat h_flipped_mat;
            flip(mat, h_flipped_mat, HORIZONTAL);
            imwrite("../horizontal_flipped_image.bmp", h_flipped_mat);

            // 创建一个新的Mat对象来存储垂直镜像后的图像
            Mat v_flipped_mat;
            flip(mat, v_flipped_mat, VERTICAL);
            imwrite("../vertical_flipped_image.bmp", v_flipped_mat);
        }
        else if (choice == 9) {
            // 加载图像
            Mat mat = imread("../lena.bmp",IMREAD_GRAYSCALE);

            if (mat.empty()) {
                std::cerr << "Error: Image cannot be loaded!" << std::endl;
                return -1;
            }

            // 定义旋转角度
            double angle = 45.0; // 旋转角度

            // 创建一个新的Mat对象来存储旋转后的图像
            Mat rotated_mat;
            rotate(mat, rotated_mat, angle);

            // 假设我们有一个保存图像的函数
            imwrite("../rotated_image.bmp", rotated_mat);
        }
        else if (choice == 0)
            std::cout
                << "已退出。\n";
        else std::cout << "输入错误，请重新输入。\n";
    }
    while (choice != 0);

    return 0;
}
