#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    Mat src, dst, src_gray;
    src = imread("/Users/zhouweijie/Desktop/123.png", 1);

    cvtColor(src, src_gray, COLOR_BGR2GRAY);

    //namedWindow(window_name, WINDOW_AUTOSIZE);

    imshow("aaaaaa", src_gray);

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