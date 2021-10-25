//
//  match.hpp
//  test
//
//  Created by 周伟杰 on 2021/9/22.
//

#ifndef match_hpp
#define match_hpp

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"

class MatchTemplate{
public:
    MatchTemplate(cv::Mat src, cv::Mat tem);
    ~MatchTemplate();
    // 将匹配到的目标的中间1/2区域覆盖掉
    void coverTaret(cv::Mat &result, cv::Point minLoc, int maxValue, int cols, int rows);
    void calculate(int nums);
    
public:
    cv::Mat src_image;
    cv::Mat tem_image;
    cv::Mat resultImage;
    
    int resultImage_cols;
    int resultImage_rows;
    
    double minValue, maxValue;
    cv::Point minLoc, maxLoc;
    
    std::vector<cv::Point> roi_image;
    
};


#endif /* match_hpp */
