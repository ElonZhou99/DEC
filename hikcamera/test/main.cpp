//
//  main.cpp
//  test
//
//  Created by 周伟杰 on 2021/9/10.
//

#include "hk_camera.hpp"
#include "match.hpp"

#include <opencv2/core/types_c.h>

#define FPS 15
#define GAIN 15

unsigned int g_nPayloadSize = 0;


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
    int x = up;
    int y = left;
    int new_clos = image.cols - up - down;
    int new_rows = image.rows - left - right;
    cv::Rect roi(x, y, new_clos, new_rows);
    image(roi).copyTo(image);
    return image;
}


int main(int argc, const char * argv[]) {
    
    HikCamera HikCamera;
    HikCamera.start(GAIN);
    HikCamera.grab_image();

    cv::Mat images;
    cv::Mat tem_image = cv::imread("/Users/zhouweijie/Desktop/2.png", 0);

    int model;
    std::cout << "please choose model (0 = match, 1 = detection) : ";
    std::cin >> model;
    
    while (model == 0)
    {
        // 图像切割
        // 预处理
        // 匹配螺栓螺母
        // 匹配结果图像分螺栓螺母两个感兴趣区域
        // 阈值截断
        // 直线拟合
        // 计算螺栓螺母的标记线的差
        // x方向的差换算为角度
        cv::Mat init_image;
        init_image = HikCamera.get_one_frame();
        init_image = HikCamera.convert2Mat(init_image);
        
        
        
        //--------------------------
        //-----------图像切割---------
        //--------------------------
        init_image = cutImage(init_image, 200, 200, 200, 200);
        
        
        
        //--------------------------
        //-----------预处理----------
        //--------------------------
        cv::Mat equalize_image;
        equalizeHist(init_image, equalize_image);
        cv::Mat medianBlur_image;
        medianBlur(init_image, medianBlur_image, 3);
        cv::Mat guss_image;
        GaussianBlur(medianBlur_image, guss_image, cv::Size(3, 3), 15, 15);

        
        
        
        //--------------------------
        //-----------匹配螺栓螺母------
        //--------------------------
        MatchTemplate Match(init_image, tem_image);
        int nums = 4;
        Match.calculate(nums);
        std::vector<cv::Mat> result_images;
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
            guss_image(roi_rect[i]).copyTo(result_images[i]);
        }
        
        
        
        
        //----------------------------
        //----------拟合直线-----------
        //----------------------------
        std::vector<cv::Point> points;
        for (int i = 0; i < result_images[0].rows; i++){
            for (int j = 0; j < result_images[0].cols; j++){
                if (result_images[0].at<float>(j, i) > 0) {
                    cv::Point temp;
                    temp.x = j;
                    temp.y = i;
                    points.push_back(temp);
                }
            }
        }
        cv::Vec4f line_para;
        cv::fitLine(points, line_para, cv::DIST_L2, 0, 1e-2, 1e-2);
        //获取点斜式的点和斜率
        cv::Point point0;
        point0.x = line_para[2];
        point0.y = line_para[3];
        double k = line_para[1] / line_para[0];
        //计算直线的端点(y = k(x - x0) + y0)
        cv::Point point1, point2;
        point1.x = 0;
        point1.y = k * (0 - point0.x) + point0.y;
        point2.x = 640;
        point2.y = k * (640 - point0.x) + point0.y;
        cv::line(result_images[0], point1, point2, cv::Scalar(0, 255, 0), 2, 8, 0);


        
        cv::namedWindow("test", cv::WINDOW_AUTOSIZE);
        mergeImg(images, result_images[0], result_images[1], result_images[2], result_images[3]);
        imshow("test", images);
        
        cv::namedWindow("init", cv::WINDOW_AUTOSIZE);
        imshow("init", init_image);

        
        if (cv::waitKey(1000 / FPS) == 27){
            cv::destroyAllWindows();
            return 0;
        }
    }
    
    while (model == 1)
    {
        // ------特殊抓边------
        // 匹配辅助检测工件
        // 根据minLoc，copy出匹配结果图像result_image
        // 在result_image中执行阈值直线检测，返回直线的中点mid_y坐标
        // y = minLoc.y + mid_y
        cv::Mat dst, cdst, cdstP;
        cv::Mat init_image;
        init_image = HikCamera.get_one_frame();
        init_image = HikCamera.convert2Mat(init_image);
        GaussianBlur(init_image, init_image, cv::Size(3, 3), 15, 15);
        threshold(init_image, init_image, 170, 255, cv::THRESH_BINARY);
        
        Canny(init_image, dst, 150, 300);
        cvtColor(dst, cdst, cv::COLOR_GRAY2BGR);
        cdstP = cdst.clone();
        
        // Standard Hough Line Transform
        std::vector<cv::Vec2f> lines; // will hold the results of the detection
        HoughLines(dst, lines, 1, CV_PI/180, 130, 0, 0 ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            cv::Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a*rho, y0 = b*rho;
            pt1.x = cvRound(x0 + 1000*(-b));
            pt1.y = cvRound(y0 + 1000*(a));
            pt2.x = cvRound(x0 - 1000*(-b));
            pt2.y = cvRound(y0 - 1000*(a));
            line( cdst, pt1, pt2, cv::Scalar(0,0,255), 3, cv::LINE_AA);
            std::cout << "x: " << pt2.x << "\n" << "y: " << pt2.y << std::endl;
        }
        

        // Probabilistic Line Transform
        std::vector<cv::Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(dst, linesP, 1, CV_PI/180, 130, 150, 10 ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            cv::Vec4i l = linesP[i];
            line( cdstP, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, cv::LINE_AA);
        }
        // Show results
        imshow("Source", init_image);
        imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst);
        imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
        
        if (cv::waitKey(1000 / FPS) == 27){
            cv::destroyAllWindows();
            return 0;
        }
        
    }
    return 0;
    
    
}




