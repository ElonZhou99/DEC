#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{

    src = imread("/Users/zhouweijie/Desktop/123.png", 1);

    Mat src, dst, src_gray;

    cvtColor(src, src_gray, CV_BGR2GRAY);

    //namedWindow(window_name, WINDOW_AUTOSIZE);

    imshow(window_name, src_gray);

    while (true)
    {
        int c;
        c = waitKey(20);
        if (27 == (char)c)
        {
            break;
        }
    }
}