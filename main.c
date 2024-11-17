#include "imgproc.h"
#include "openHL.h"
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
    printf("4. 生成灰度直方图\n");
    printf("0. 退出\n");
    printf("===========================\n");
    printf("请输入数字来运行功能：");
}

int main(int argc, char* argv[])
{
    unsigned char choice;
    Mat           in, out;

    do {
        showMenu();
        scanf("%hhu", &choice);
        switch (choice)
        {
            case 1 :
                // 转换为灰度图像
                in = imread("rgb.bmp");
                cvtColor(&in, &out, COLOR_BGR2GRAY);
                imwrite("rgb_gray.bmp", &out);
                printf("已将图像转换为灰度图，保存为 rgb_gray.bmp\n");
                deleteMat(&in);
                deleteMat(&out);
                break;
            case 2 :
                // 转换为反转灰度图像
                in = imread("rgb.bmp");
                cvtColor(&in, &out, COLOR_BGR2GRAY_INVERTED);
                imwrite("rgb_gray_inverted.bmp", &out);
                printf("已将图像转换为反转灰度图，保存为 rgb_gray_inverted.bmp\n");
                deleteMat(&in);
                deleteMat(&out);
                break;
            case 3 :
                // 分离通道
                in = imread("rgb.bmp");
                Mat out_array[3];
                split(&in, out_array);
                imwrite("rgb_B.bmp", &out_array[0]);
                imwrite("rgb_G.bmp", &out_array[1]);
                imwrite("rgb_R.bmp", &out_array[2]);
                printf("已分离图像通道，分别保存为 rgb_B.bmp, rgb_G.bmp, rgb_R.bmp\n");
                deleteMat(&in);
                deleteMat(&out_array[0]);
                deleteMat(&out_array[1]);
                deleteMat(&out_array[2]);
                break;
            case 4 :
                // 生成灰度直方图
                in = imread("temp.bmp");
                cvtColor(&in, &out, COLOR_BGR2GRAY);
                // Mat hist;
                // calcHist(&in, &hist);
                // Mat hist_norm;
                // normalize(&hist, &hist_norm, 0, 1000, NORM_MINMAX);
                // Mat hist_norm_img;
                // drawHist(&hist_norm, &hist_norm_img, 10, 1100);
                // imwrite("dim_hist_norm_img.bmp", &hist_norm_img);
                // printf("已生成灰度直方图，保存为 dim_hist_norm_img.bmp\n");

                equalizeImage(&out);
                imwrite("dim_equalized.bmp", &out);




                // deleteMat(&in);
                // deleteMat(&out);
                // deleteMat(&hist);
                // deleteMat(&hist_norm);
                // deleteMat(&hist_norm_img);
                break;
            case 0 :
                printf("退出程序。\n");
                break;
            default :
                printf("无效输入，请重新选择。\n");
        }
    }
    while (choice != 0);

    return 0;
}
