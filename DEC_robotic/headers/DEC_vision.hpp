//
//  DEC_robotic.hpp
//  robotic_version
//
//  Created by 周伟杰 on 2021/7/18.
//

#ifndef DEC_robotic_hpp
#define DEC_robotic_hpp

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>

#include <opencv2/video/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <pylon/PylonIncludes.h>
#include <pylon/_BaslerUniversalCameraParams.h>

#include "cvui.h"
#define CVUI_IMPLEMENTATION


using namespace Pylon;
using namespace cv;
using namespace std;

class Vision{
public:
    Vision();
    ~Vision();
    
    void colorButton(Mat initVideo);
    void changeHSV(string color);
    int shapeButton(Mat initVideo, int approxSize);
    
    void trackbar();
    void trackbarChange(int hsv, int i);
    
    Point getCenterPoint(Rect rect);
    Mat videoTransform(Mat initVideo);
    vector<vector<Point>> findShape(vector<vector<Point>>contours, vector<vector<Point>> shapeContours, int approxSize);
    Rect maxArea(vector<vector<Point>> contours, Rect rect, Point2f center);
            
private:
    int Hmin = 0;
    int Smin = 43;
    int Vmin = 46;
    int Hmax = 10;
    int Smax = 255;
    int Vmax = 255;
    
};




#endif /* DEC_robotic_hpp */
