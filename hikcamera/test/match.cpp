//
//  match.cpp
//  test
//
//  Created by 周伟杰 on 2021/9/22.
//

#include "match.hpp"

MatchTemplate::MatchTemplate(cv::Mat src, cv::Mat tem){
    src_image = src;
    tem_image = tem;
    resultImage_cols = src_image.cols - tem_image.cols + 1;
    resultImage_rows = src_image.rows - tem_image.rows + 1;
    resultImage.create(resultImage_cols, resultImage_rows, CV_32FC1);
    
}

MatchTemplate::~MatchTemplate() {
    
}

void MatchTemplate::coverTaret(cv::Mat &result, cv::Point minLoc, int maxVaule, int cols, int rows) {
    int startX = minLoc.x - (cols / 4);
    int startY = minLoc.y - (rows / 4);
    
    int endX = minLoc.x + (cols);
    int endY = minLoc.y + (rows);
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            result.at<float>(y, x) = 0.8;
        }
    }
}

void MatchTemplate::calculate(int nums){
    matchTemplate(src_image, tem_image, resultImage, 1);
    minMaxLoc(resultImage, &minValue, &maxValue, &minLoc, &maxLoc, cv::Mat());
    roi_image.push_back(minLoc);
    rectangle(src_image, minLoc, cv::Point(minLoc.x + tem_image.cols, minLoc.y + tem_image.rows), cv::Scalar(0, 0, 255), 5, 8, 0);
    
    for (int i = 1; i < nums; i++) {
        //matchTemplate(src_image, tem_image, resultImage, 1);
        coverTaret(resultImage, minLoc, maxValue, tem_image.cols, tem_image.rows);
        minMaxLoc(resultImage, &minValue, &maxValue, &minLoc, &maxLoc, cv::Mat());
        roi_image.push_back(minLoc);
        rectangle(src_image, minLoc, cv::Point(minLoc.x + tem_image.cols, minLoc.y + tem_image.rows), cv::Scalar(0, 125, 0), 5, 8, 0);
    }
}
