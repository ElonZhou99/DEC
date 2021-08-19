//
//  DEC_robotic.cpp
//  robotic_version
//
//  Created by 周伟杰 on 2021/7/18.
//

#include "DEC_vision.hpp"

Vision::Vision(){
    
}

Vision::~Vision(){
    
}
    
Point Vision::getCenterPoint(Rect rect) {
    Point cpt;
    cpt.x = rect.x + cvRound(rect.width / 2.0);
    cpt.y = rect.y + cvRound(rect.height / 2.0);
    return cpt;
}


/*          black   white   red    orange   yellow  blue    purple  green
 * Hmin     0       0       0       11      26      100     125     35
 * Smin     0       0       43      43      43      43      43      43
 * Vmin     0       211     46      46      46      46      46      46
 * Hmax     180     180     10      25      34      124     155     77
 * Smax     255     30      255     255     255     255     255     255
 * Vmax     46      255     255     255     255     255     255     255
 */
void Vision::changeHSV(string color) {
    if (color == "whi") {
        Hmin = 50;
        Smin = 50;
        Vmin = 100;
        Hmax = 100;
        Smax = 100;
        Vmax = 150;
    }
    else if (color == "red") {
        Hmin = 0;
        Smin = 43;
        Vmin = 46;
        Hmax = 10;
        Smax = 255;
        Vmax = 255;
    }
    else if (color == "yel") {
        Hmin = 30;
        Smin = 220;
        Vmin = 130;
        Hmax = 60;
        Smax = 255;
        Vmax = 170;
    }
    else if (color == "lig") {
        Hmin = 0;
        Smin = 0;
        Vmin = 211;
        Hmax = 180;
        Smax = 30;
        Vmax = 255;
    }
}

void Vision::colorButton(Mat initVideo) {
    if (cvui::button(initVideo, 200, 100, 200, 80, "red")) {
        Hmin = 0;
        Smin = 43;
        Vmin = 46;
        Hmax = 10;
        Smax = 255;
        Vmax = 255;
    }
    else if (cvui::button(initVideo, 550, 100, 200, 80,  "blue")) {
        Hmin = 100;
        Smin = 43;
        Vmin = 46;
        Hmax = 124;
        Smax = 255;
        Vmax = 255;
    }
    else if (cvui::button(initVideo, 900, 100, 200, 80,  "white")) {
        Hmin = 0;
        Smin = 0;
        Vmin = 211;
        Hmax = 180;
        Smax = 30;
        Vmax = 255;
    }
    else if (cvui::button(initVideo, 1250, 100, 200, 80,  "green")) {
        Hmin = 35;
        Smin = 140;
        Vmin = 120;
        Hmax = 55;
        Smax = 255;
        Vmax = 255;
    }
    else if (cvui::button(initVideo, 1600, 100, 200, 80,  "yellow")) {
        Hmin = 26;
        Smin = 43;
        Vmin = 46;
        Hmax = 34;
        Smax = 255;
        Vmax = 255;
    }
    cvui::update();
}

int Vision::shapeButton(Mat initVideo, int approxSize) {
    if (cvui::button(initVideo, 100, 150, 100, 50, "rectangle")) {
        approxSize = 4;
    }
    else if (cvui::button(initVideo, 300, 150, 100, 50,  "circular")) {
        approxSize = 0;
    }
    else if (cvui::button(initVideo, 500, 150, 100, 50,  "triangle")) {
        approxSize = 3;
    }
    return approxSize;
}



Mat Vision::videoTransform(Mat initVideo){

    Mat gussVideo;
    GaussianBlur(initVideo, gussVideo, Size(5, 5), 0);
    Mat HSVVideo;
    cvtColor(gussVideo, HSVVideo, COLOR_BGR2HSV);
    /*
    vector<Mat> vec_channels;
    split(HSVVideo, vec_channels);
    equalizeHist(vec_channels[0], vec_channels[0]);
    merge(vec_channels, HSVVideo);
    */
    Mat thresholderVideo;
    inRange(HSVVideo, Scalar(Hmin, Smin, Vmin), Scalar(Hmax, Smax, Vmax), thresholderVideo);
    Mat outVedio;
    erode(thresholderVideo, outVedio, getStructuringElement(MORPH_RECT, Size(5, 5)));
    dilate(thresholderVideo, outVedio, getStructuringElement(MORPH_RECT, Size(6, 6)));
    // inRange(HSVVideo, colorSet[0], colorSet[1], thresholderVideo);
    return outVedio;
}

vector<vector<Point>> Vision::findShape(vector<vector<Point>>contours, vector<vector<Point>> shapeContours, int approxSize)
    {
    
    vector<Point> approx;
    if (contours.size() > 0) {
        for (int i = 0; i < contours.size(); i++) {
            approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
            //cout << i << "***" << approx.size() << endl;
            if (approx.size() == approxSize) {
                shapeContours.push_back(contours[i]);
                //cout << i << "***" << shapeContours.size() << "**" << approx.size()<< endl;
            }
        }
    }
    
    return shapeContours;
}


Rect Vision::maxArea(vector<vector<Point>> contours, Rect rect, Point2f center) {
    float radius = 2;
    if (contours.size() > 0)
    {
        double maxArea=0;
        for (int i = 0; i < contours.size(); i++)
        {
            double area = fabs(contourArea(contours[i]));
            if (area > maxArea)
            {
                maxArea = area;
                rect = boundingRect(contours[i]);
                minEnclosingCircle(contours[i], center, radius);
            }
        }
    }
    return rect;
}
