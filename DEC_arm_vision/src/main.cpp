//
//  main.cpp
//  robotic_version
//
//  Created by 周伟杰 on 2021/7/18.
//


#include "DEC_vision.hpp"
#include "cvui.h"
#include "socket.hpp"
#include <sys/time.h>

#define CVUI_IMPLEMENTATION
#define FPS 40

int main(int argc, char** argv) {
    
    /*
     * basler cam initial
     */
    CImageFormatConverter formatConverter;
    formatConverter.OutputPixelFormat = PixelType_BGR8packed;
    CPylonImage pylonImage;
    PylonInitialize();
    CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());
    camera.StartGrabbing(GrabStrategy_LatestImageOnly);
    CGrabResultPtr ptrGrabResult;

    Vision Robotic;
    cvui::init("vision");
    int sideNums = 4; // target block's sides num initial
    
    
    /*
     * calculate time
     */
    int count = 0;
    clock_t startTime;
    startTime = clock();
    
    
    /*
     * socket start
     */
    Client Client1;
    Client1.clientLink();
    Client1.clientWrite();
    string color = Client1.clientRead();
    Robotic.changeHSV(color);
    
    while(camera.IsGrabbing()) {
        
        camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
        if (ptrGrabResult->GrabSucceeded()){
            /*
             * transform CPylonImage to Mat
             */
            Mat initVideo;
            formatConverter.Convert(pylonImage, ptrGrabResult);
            initVideo = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());
            

            
            /*
             * target color & shape control
             */
            sideNums = Robotic.shapeButton(initVideo, sideNums);
            // Robotic.colorButton(initVideo);
            
            Mat transformVideo = Robotic.videoTransform(initVideo);
            
            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            findContours(transformVideo, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
            
            //contours = Robotic.findShape(contours, contours, sideNums);

            /*
             * select the max block
             */
            Rect targetBoundary;
            Point2f center;
            targetBoundary = Robotic.maxArea(contours, targetBoundary, center);
            Point centerPoint = Robotic.getCenterPoint(targetBoundary);
            
            /*
             * dispalay
             */
            rectangle(initVideo, targetBoundary, Scalar(0,255,0),5);
            circle(initVideo, centerPoint, 12, Scalar(0,0,255), -1);
            
            namedWindow("vision", WINDOW_NORMAL);
            resizeWindow("vision", 880, 660);
            cvui::imshow("vision", initVideo);
            //namedWindow("transformVideo", WINDOW_NORMAL);
            //resizeWindow("transformVideo", 800, 630);
            //imshow("transformVideo", transformVideo);
            
            /*
             * socket to robot
             */
            Client1.clientWrite(centerPoint);
            color = Client1.clientRead();
            Robotic.changeHSV(color);
            
            
            if (waitKey(1000 / FPS) == 27){
                destroyAllWindows();
                break;
            }
        }
        
        count ++;
        clock_t oneTime;
        oneTime = clock();
        printf("times%d: %f\n", count, (double)(oneTime-startTime)/(double)CLOCKS_PER_SEC) ;
    }
    Client1.clientClose();
    return 0;
}


