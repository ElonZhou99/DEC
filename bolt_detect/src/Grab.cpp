#include "match.hpp"
#include "socket.hpp"
#include "TCPClient.hpp"

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include <opencv2/video/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#define FPS 20

TCPClient tcp;


using namespace Pylon;
using namespace std;
using namespace cv;


void sig_exit(int s)
{
        tcp.exit();
        exit(0);
}



Point getCenterPoint(Rect rect) {
    Point cpt;
    cpt.x = rect.x + cvRound(rect.width / 2.0);
    cpt.y = rect.y + cvRound(rect.height / 2.0);
    return cpt;
}


void mergeImg(cv::Mat & dst,cv::Mat &src1,cv::Mat &src2, cv::Mat &src3, cv::Mat &src4)
{
    int rows = src1.rows+5+src2.rows;
    int cols = src1.cols+5+src2.cols;
    CV_Assert(src1.type () == src2.type ());
    dst.create(rows,cols,src1.type ());
    src1.copyTo(dst(cv::Rect(0,0,src1.cols,src1.rows)));
    src2.copyTo(dst(cv::Rect(src1.cols+5,0,src2.cols,src2.rows)));
    src3.copyTo(dst(cv::Rect(0,src1.rows+5,src3.cols,src3.rows)));
    src4.copyTo(dst(cv::Rect(src1.cols+5,src1.rows+5,src4.cols,src4.rows)));
}


cv::Mat cutImage(cv::Mat image, int up, int down, int left, int right){
    int x = left;
    int y = up;
    int new_clos = image.cols - left - right;
    int new_rows = image.rows - up - down;
    cv::Rect roi(x, y, new_clos, new_rows);
    image(roi).copyTo(image);
    return image;
}


vector<Point> getPoints(Mat &image, int value) {
    int nl = image.rows;
    int nc = image.cols * image.channels();
    vector<Point> points;
    for (int j = 0; j < nl; j++){
        uchar* data = image.ptr<uchar>(j);
        for (int i = 0; i < nc; i++){
            if (data[i] == value){
                points.push_back(Point(i, j));
            }
        }
    }
    return points;
}


float averageX(vector<Point> points){
    vector<float> points_x;
    int nums = points.size();
    for (int i = 0; i < nums; i++){
        points_x.push_back(points[i].x);
    }
    sort(points_x.begin(), points_x.end());
    float averageX;
    int refuse = 120;
    int sum = 0;
    for (int i = refuse; i < nums - refuse; i++) {
        sum += points_x[i];
    }
    averageX = sum / (nums - 2*refuse);
    return averageX;
}


vector<float> detection(cv::Mat tem_image, cv::Mat init_image, vector<float> vec_ans){
    // ------------------------------------------
    // -----------------------------------------
    // 输出init_image和gray_image
    // init_image rgb图，gray_image 灰度图
    // ------------------------------------------
    // ------------------------------------------
    Mat gray_image = init_image.clone();
    cvtColor(gray_image, gray_image, CV_BGR2GRAY);
    
    init_image = cutImage(init_image, 100, 500, 0, 0);
    gray_image = cutImage(gray_image, 100, 500, 0, 0);
    
    cv::Mat equalize_image;
    //equalizeHist(init_image, init_image);
    cv::Mat medianBlur_image;
    //medianBlur(init_image, medianBlur_image, 3);
    //medianBlur(img1, img1, 3);
    cv::Mat guss_image;
    GaussianBlur(init_image, init_image, cv::Size(3, 3), 0, 0);
    GaussianBlur(gray_image, gray_image, cv::Size(3, 3), 0, 0);

    
    
    // -----------------------------------------
    // -----------------------------------------
    // 匹配螺栓螺母
    // 输出 vector<Mat> result_images
    // -----------------------------------------
    // -----------------------------------------
    MatchTemplate Match(gray_image, tem_image);
    int nums = 1;
    Match.calculate(nums);
    std::vector<cv::Mat> result_images; // 由init_image输出的结果图
    std::vector<cv::Rect> roi_rect;
    for (int i = 0; i < nums; i++) {
        int x = Match.roi_image[i].x;
        int y = Match.roi_image[i].y;
        int cols = Match.tem_image.cols;
        int rows = Match.tem_image.rows;
        cv::Rect roi(x, y, cols, rows);
        roi_rect.push_back(roi);
        cv::Mat result_image;
        result_images.push_back(result_image);
        init_image(roi_rect[i]).copyTo(result_images[i]);
    }
    
    
    
    // -----------------------------------------
    // -----------------------------------------
    // 裁剪出螺栓螺母的rgb图
    // bolt_image 螺栓区域图片，nut_image 螺母区域
    // ----------------------------------------
    // -----------------------------------------
    cv::Mat bolt_image, nut_image;
    int boltLenth = 420;
    cv::Rect bolt_roi(0, 0, result_images[0].cols, boltLenth);
    cv::Rect nut_roi(0, boltLenth, result_images[0].cols, result_images[0].rows - boltLenth);
    result_images[0](bolt_roi).copyTo(bolt_image);
    result_images[0](nut_roi).copyTo(nut_image);
    
    
    
    // -----------------------------------------
    // -----------------------------------------
    // findColor
    // -----------------------------------------
    // ----------------------------------------
    inRange(bolt_image, Scalar(23, 30, 45), Scalar(51, 55, 70), bolt_image);
    inRange(nut_image, Scalar(23, 30, 45), Scalar(51, 55, 70), nut_image);
    
    //erode(bolt_image, bolt_image, getStructuringElement(MORPH_RECT, Size(5, 5)));
    dilate(bolt_image, bolt_image, getStructuringElement(MORPH_RECT, Size(5, 5)));
    
    dilate(nut_image, nut_image, getStructuringElement(MORPH_RECT, Size(5, 5)));
    erode(nut_image, nut_image, getStructuringElement(MORPH_RECT, Size(5, 5)));
    

    
    // ----------------------------------------
    // -----------------------------------------
    // bolt_points螺栓划线点，nut_points螺母划线点
    // 输出差值delta_x
    // ----------------------------------------
    // -----------------------------------------
    vector<cv::Point> bolt_points;
    vector<cv::Point> nut_points;
    bolt_points = getPoints(bolt_image, 255);
    nut_points = getPoints(nut_image, 255);
    //cout << "bolt_points : " << bolt_points.size() << "  nut_points :  " << nut_points.size() << endl;
    
    float bolt_x, nut_x;
    bolt_x = averageX(bolt_points);
    nut_x = averageX(nut_points);
    float delta_x = bolt_x - nut_x;
    vec_ans.push_back(delta_x);
    
    
    cv::namedWindow("init_image", cv::WINDOW_AUTOSIZE);
    imshow("init_image", gray_image);
    
    cv::namedWindow("bolt_image", cv::WINDOW_AUTOSIZE);
    imshow("bolt_image", bolt_image);
    
    cv::namedWindow("nut_image", cv::WINDOW_AUTOSIZE);
    imshow("nut_image", nut_image);
    
    return vec_ans;
}


int main( int /*argc*/, char* /*argv*/[] )
{
    vector<float> init_delta = {-9.5, 10.7, 10.7, 10.7, 10.7, 10.7, 10.7, 10.7, 10.7, 10.7};
    
    cv::Mat images;
    cv::Mat tem_image = cv::imread("/Users/zhouweijie/Desktop/1.png", 0);
    //cout << tem_image.cols << endl;

    CImageFormatConverter formatConverter;
    formatConverter.OutputPixelFormat = PixelType_BGR8packed;
    CPylonImage pylonImage;
    PylonInitialize();
    CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
    camera.StartGrabbing(GrabStrategy_LatestImageOnly);
    CGrabResultPtr ptrGrabResult;
    camera.IsGrabbing();
    
    /*
    Client socket_inter;
    Client socket_out;
    socket_inter.clientLink("127.0.0.1", 9999);
    socket_out.clientLink("127.0.0.1", 12345);
     */
    tcp.setup("127.0.0.1", 9999);
    //tcp.setup("127.0.0.1", 8888);
    
    
    int which = 0;
    while(1){
        int angle = 36;
        int no = 0;
        
        for(int a =0; a < 360; a += angle){
            // ----------------------------------------
            // ----------------------------------------
            // 单个螺栓检测，20张图片一组，去头去尾求平均得ans
            // ----------------------------------------
            // ----------------------------------------
            //socket_inter.clientWrite(no);
            //string OK = socket_inter.clientRead();
            tcp.Send(to_string(no));
            //string rec = tcp.receive();
            //cout << rec << endl;
            vector<float> vec_ans;
            float ans = 0;
            // 检测次数
            int img_nums = 20;
            for(int j = 0; j < img_nums; j ++) {
                camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
                if (ptrGrabResult->GrabSucceeded()){
                    cv::Mat init_image;
                    formatConverter.Convert(pylonImage, ptrGrabResult);
                    init_image = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
                    imshow("a", init_image);
                    
                    // vec_ans： 求出的20张图片的螺栓螺母delta值
                    vec_ans = detection(tem_image, init_image, vec_ans);

                    if (cv::waitKey(1000 / FPS) == 27){
                        cv::destroyAllWindows();
                        return 0;
                    }
                }

            }
            
            // 将求的20张图片的vec_ans数组delta值去头去尾，求平均值ans
            sort(vec_ans.begin(), vec_ans.end());
            for(int i = 2; i < img_nums - 2; i ++){
                ans = ans + vec_ans[i];
            }
            ans = ans / (img_nums - 4);
            
            ans = (init_delta[which] - ans) * 0.175; // 计算和初始值的差值，计算初始值只需要注释改语句
            cout << "no" << no << ": " << ans << endl;
            no += 1;
            tcp.Send(to_string(ans));
        }
        //sleep (5);
        //cout << "change: " << endl;
        //cin >> which;
    }
    return 0;
}
