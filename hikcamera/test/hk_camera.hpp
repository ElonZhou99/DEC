//
//  hk_camera.hpp
//  DEC_BOLT_INSPECTION
//
//  Created by ZhouWeijie on 2021/9/9.
//  email: elonzhou99@gmail.com
//

#ifndef hk_camera_hpp
#define hk_camera_hpp

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"


#include "MvCameraControl.h"

class HikCamera
{
public:
    // for INFO
    
    
    void *handle = NULL;
    unsigned char *pData = NULL;
    int nRet;
    MVCC_INTVALUE stParam;
    MV_FRAME_OUT_INFO_EX pstImageInfo;
    MV_CC_PIXEL_CONVERT_PARAM stConvertParam;
    bool bConvertRet;
    
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};

    HikCamera();
    ~HikCamera();
    void start(float gain);
    bool grab_image();
    bool get_one_frame();
    cv::Mat convert2Mat(cv::Mat srcImage);
    void shutdown();
    void stop_grab_image();
    void close_device();

private:
    bool PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo);
};

#endif /* hk_camera_hpp */
