#include "openHL/openHL.h"
#include <stdio.h>

// 功能菜单
void showMenu()
{
    printf("\n===========================\n");
    printf("        图像处理工具        \n");
    printf("===========================\n");
    printf("请选择功能：\n");
    printf("1. 将图像转换为灰度图\n");
    printf("2. 将图像转换为反转灰度图\n");
    printf("3. 分离图像通道\n");
    printf("4. 直方图u处理\n");
    printf("0. 退出\n");
    printf("===========================\n");
    printf("请输入数字来运行功能：");
}

int main()
{
    unsigned char choice;

    do {
        showMenu();
        scanf("%hhu", &choice);

        if (choice == 1)
        {
            // 转换为灰度图像
            Mat rgb, rgb_gray;
            rgb = imread("/home/LuoTat/Desktop/Digital_Image_Processing/rgb.bmp", IMREAD_UNCHANGED);
            cvtColor(&rgb, &rgb_gray, COLOR_BGR2GRAY);
            imwrite("/home/LuoTat/Desktop/Digital_Image_Processing/rgb_gray.bmp", &rgb_gray);
            printf("已将图像转换为灰度图，保存为 rgb_gray.bmp\n");
            release(&rgb);
            release(&rgb_gray);
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
        // else if (choice == 3)
        // {
        //     // 分离通道
        //     Mat rgb;
        //     Mat splited_array[3];
        //     rgb = imread("rgb.bmp");
        //     split(&rgb, splited_array);
        //     imwrite("rgb_B.bmp", &splited_array[0]);
        //     imwrite("rgb_G.bmp", &splited_array[1]);
        //     imwrite("rgb_R.bmp", &splited_array[2]);
        //     printf("已分离图像通道，分别保存为 rgb_B.bmp, rgb_G.bmp, rgb_R.bmp\n");
        //     deleteMat(&rgb);
        //     deleteMat(&splited_array[0]);
        //     deleteMat(&splited_array[1]);
        //     deleteMat(&splited_array[2]);
        // }
        // else if (choice == 4)
        // {
        //     // 直方图处理
        //     Mat dim, dim_hist, dim_hist_norm, dim_hist_norm_img;
        //     Mat dim_equalized, dim_equalized_hist, dim_equalized_hist_norm, dim_equalized_hist_norm_img;
        //     dim = imread("dim.bmp");
        //     calcHist(&dim, &dim_hist);
        //     normalize(&dim_hist, &dim_hist_norm, 0, 1000, NORM_MINMAX);
        //     drawHist(&dim_hist_norm, &dim_hist_norm_img, 10, 1100);
        //     imwrite("dim_hist_norm_img.bmp", &dim_hist_norm_img);
        //     printf("已生成灰度直方图，保存为 dim_hist_norm_img.bmp\n");

        //     equalizeImage(&dim);
        //     imwrite("dim_equalized.bmp", &dim);
        //     printf("已生成均衡化图像，保存为 dim_equalized.bmp\n");
        //     calcHist(&dim, &dim_equalized_hist);
        //     normalize(&dim_equalized_hist, &dim_equalized_hist_norm, 0, 1000, NORM_MINMAX);
        //     drawHist(&dim_equalized_hist_norm, &dim_equalized_hist_norm_img, 10, 1100);
        //     imwrite("dim_equalized_hist_norm_img.bmp", &dim_equalized_hist_norm_img);
        //     printf("已生成直方图均衡化图像，保存为 dim_equalized_hist_norm_img.bmp\n");


        //     deleteMat(&dim);
        //     deleteMat(&dim_hist);
        //     deleteMat(&dim_hist_norm);
        //     deleteMat(&dim_hist_norm_img);
        //     deleteMat(&dim_equalized);
        //     deleteMat(&dim_equalized_hist);
        //     deleteMat(&dim_equalized_hist_norm);
        //     deleteMat(&dim_equalized_hist_norm_img);
        // }
        else if (choice == 0)
        {
            printf("退出程序。\n");
        }
        else printf("无效输入，请重新选择。\n");
    }
    while (choice != 0);

    return 0;
}
