//
//  main.cpp
//  test
//
//  Created by 周伟杰 on 2021/9/10.
//
#include <time.h>
#include <map>

#include <opencv2/core/types_c.h>
#include <opencv2/calib3d/calib3d.hpp>

//#include "hk_camera.hpp"
#include "txt_option.hpp"
#include "socket.hpp"

#define FPS 50
//#define GAIN 12
#define INFO_FILE "/home/nvidia/detect_jetson/info.txt"
#define SERV_IP "192.168.10.103"
#define PORT 9999

unsigned int g_nPayloadSize = 0;
using namespace std;
using namespace cv;

float detect(Mat imageInput, float initY, int initChoose)
{

    float averageY = 0;
    Size board_size = Size(11, 8);    // 标定板行列角点数
    vector<Point2f> image_points_buf; // 缓存每幅图像检测的角点

    // 提取角点
    bool patternfound = findChessboardCorners(imageInput, board_size, image_points_buf);
    if (!patternfound)
    {
        cout << "can not find chessboard corners!\n"
             << endl;
    }
    else
    {
        Mat view_gray;
        // cvtColor(imageInput, view_gray, COLOR_RGB2GRAY);  // 转灰度图
        /* 亚像素精确化 */
        // image_points_buf 初始的角点坐标向量，同时作为亚像素坐标位置的输出
        // Size(5,5) 搜索窗口大小
        // （-1，-1）表示没有死区
        // TermCriteria 角点的迭代过程的终止条件, 可以为迭代次数和角点精度两者的组合
        cornerSubPix(imageInput, image_points_buf, Size(5, 5), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

        /* 在图像上显示角点位置 */
        // drawChessboardCorners(imageInput, board_size, image_points_buf, false); // 用于在图片中标记角点

        // --------------------------------------------------------------
        // ---------------------------计算-------------------------------
        // --------------------------------------------------------------
        vector<Point_<float>> points1 = image_points_buf;
        int nums = points1.size();

        float allY = 0;
        Point_<float> allPoint;
        for (int i = 0; i <= nums; i++)
        {
            allY += points1[i].y;
        }
        allY = allY / 88;

        if (initChoose == 1)
        {
            averageY = allY;
        }
        else
        {
            averageY = (allY - initY);
        }

        // 1  2  3  4  5  6  7  8  9  10 11
        // 12 13 14 15 16 17 18 19 20 21 22
        // 23 24 25 26 27 28 29 30 31 32 33
        // 34 35 36 37 38 39 40 41 42 43 44
        float trueDistanse = 15.0;
        // circle(imageInput, points1[39], 1, (0, 255, 0));
        // circle(imageInput, points1[40], 1, (0, 255, 0));
        // cout << "39" << points1[28] <<endl;
        // cout << "40" << points1[39] <<endl;

        float imageDistanse = points1[39].y - points1[28].y;
        // cout << "distance" << imageDistanse << endl;
        float scale = 0;
        scale = imageDistanse / trueDistanse;
        // cout << " scale" << scale << endl;
        if (initChoose == 0)
        {
            averageY = averageY / scale;
        }
        return averageY;
    }
    return 0;
}

int main(int argc, const char *argv[])
{

    TxtOption TxtOption;
    TxtOption.fileName = INFO_FILE;

    Client socket;
    socket.IP = SERV_IP;
    socket.SERV_PORT = PORT;
    socket.clientLink();

    /*
    HikCamera HikCamera;
    HikCamera.start(GAIN);
    HikCamera.grab_image();
    */

    int INIT_CHOOSE = 0;
    string info_INIT_CHOOSE = "please choose model to detect or init. (0 = detect , 1 = init)";
    std::cout << info_INIT_CHOOSE << endl;
    socket.clientWrite(info_INIT_CHOOSE);
    cin >> INIT_CHOOSE;

    int model;
    string info_model = "please choose model (3 = usbCam检测， 4 = usbCam对焦) : ";
    std::cout << info_model << endl;
    socket.clientWrite(info_model);
    std::cin >> model;

    int pointNO;
    string info_point = "please choose whice point to detect. ";
    std::cout << info_point << endl;
    socket.clientWrite(info_point);
    std::cin >> pointNO;
    pointNO = pointNO - 1;

    map<int, float> initY_map;
    if (INIT_CHOOSE == 0)
    {
        for (int i = 0; i < 20; i++)
        {
            char data[1024];
            char *info = TxtOption.ReadLineData(i + 1, data);
            initY_map[i] = atof(info);
        }
    }
    float initY = initY_map[pointNO];

    for (int i = 0; i < 20; i++)
    {
        cout << "line" << i << ":  " << initY_map[i] << endl;
    }

    // -------------------------------------------------------------------------------------------
    // ------------------------------------  -------------------------------------------------
    // -------------------------------------------------------------------------------------------
    if (model != 3 && model != 4)
    {
        cout << "model choosed erro" << endl;
    }

    /*
    // -------------------------------------------------------------------------------------------
    // --------------------------------- HikCam检测 -----------------------------------------------
    // -------------------------------------------------------------------------------------------
    while (model == 1)
    {

        Mat init_image;

        init_image = HikCamera.get_one_frame();
        init_image = HikCamera.convert2Mat(init_image);
        GaussianBlur(init_image, init_image, cv::Size(3, 3), 15, 15);
        imshow("a", init_image);

        Mat imageInput;
        init_image.copyTo(imageInput);

        float deltaY;
        deltaY = detect(imageInput, initY, INIT_CHOOSE);
        string init_y = to_string(deltaY);

        if (INIT_CHOOSE == 1){
            char dataY[1024];
            for (int i=0; i<init_y.size(); i++){
                dataY[i] = init_y[i];
            }
            TxtOption.ModifyLineData(pointNO, dataY);
        }

        string info_deltaY = "delta is " + init_y;
        cout << info_deltaY << endl;
        socket.clientWrite(info_deltaY);
        imshow("Camera Calibration", imageInput);       // 显示图

        if (cv::waitKey(1000 / FPS) == 27){
            cv::destroyAllWindows();
            return 0;
        }
    }


    while (model == 2){
        cv::Mat init_image, threshold_image;
        //HikCamera.start(GAIN);
        init_image = HikCamera.get_one_frame();
        init_image = HikCamera.convert2Mat(init_image);
        GaussianBlur(init_image, init_image, cv::Size(3, 3), 15, 15);
        imshow("a", init_image);
        if (cv::waitKey(1000 / FPS) == 27){
            cv::destroyAllWindows();
            return 0;
        }
    }
    */

    // -------------------------------------------------------------------------------------------
    // --------------------------------- usbCam检测 -----------------------------------------------
    // -------------------------------------------------------------------------------------------
    while (model == 3)
    {
        VideoCapture capture(0);
        Mat init_image;
        while (capture.isOpened())
        {
            capture >> init_image;
            Mat imageInput;
            init_image.copyTo(imageInput);
            cvtColor(imageInput, imageInput, COLOR_RGB2GRAY); // 转灰度图
            GaussianBlur(init_image, init_image, cv::Size(3, 3), 15, 15);

            float deltaY;
            deltaY = detect(imageInput, initY, INIT_CHOOSE);
            string init_y = to_string(deltaY);

            if (INIT_CHOOSE == 1)
            {
                char dataY[1024];
                for (int i = 0; i < init_y.size(); i++)
                {
                    dataY[i] = init_y[i];
                }
                TxtOption.ModifyLineData(pointNO + 1, dataY);
            }

            string info_deltaY = "delta is " + init_y;
            cout << info_deltaY << endl;
            socket.clientWrite(info_deltaY);
            //imshow("Camera Calibration", imageInput); // 显示图

            if (cv::waitKey(1000 / FPS) == 27)
            {
                cv::destroyAllWindows();
                return 0;
            }
        }
    }

    // -------------------------------------------------------------------------------------------
    // --------------------------------- usbCam对焦 -----------------------------------------------
    // -------------------------------------------------------------------------------------------
    while (model == 4)
    {
        VideoCapture capture(0);
        if (capture.isOpened())
        {
            cout << "success" << endl;
        }
        Mat init_image;
        while (capture.isOpened())
        {
            capture >> init_image;
            imshow("aaa", init_image);
            if (cv::waitKey(1000 / FPS) == 27)
            {
                cv::destroyAllWindows();
                return 0;
            }
        }
    }

    return 0;
}
