#include "opticalflowhornschunck.h"

#include "opencv/highgui.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>

// Desired number of features to trac by the optical flow algorithm.
#define FEATURE_NUM 200

OpticalFlowHornSchunck::OpticalFlowHornSchunck()
{
    // Define Options
    config.addParameter("Iterations", ConfigValueDescription("Iterations", 0, 0, 10)); // Beschreibung der Option festlegen
    config.setValue("Iterations", 2);   // Default wert einstellen

    config.addParameter("Alpha", ConfigValueDescription("Alpha", 2, 0, 10)); // Beschreibung der Option festlegen
    config.setValue("Alpha", 5.0f);
}

char* OpticalFlowHornSchunck::getName()
{
    return "Horn–Schunck";
}

cv::Mat OpticalFlowHornSchunck::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    int iterations = config.getValue("Iterations"); // Optionswert lesen
    float alpha = config.getValue("Alpha"); // Optionswert lesen

    cv::Mat pic1, pic2, pic3;
    cv::cvtColor(lastImage,     pic1, CV_BGR2GRAY);
    cv::cvtColor(currentImage,  pic2, CV_BGR2GRAY);
    currentImage.copyTo(pic3);

    //compute derivates
    cv::Mat fy1;
    cv::Mat fy2;
    cv::Mat fx1;
    cv::Mat fx2;
    cv::Mat ft1;
    cv::Mat ft2;
    cv::Mat fyKernel = (cv::Mat_<char>(2,2) << -1, -1, 1, 1);
    cv::Mat fxKernel = (cv::Mat_<char>(2,2) << -1, 1, -1, 1);
    cv::Mat ftKernel1 = (cv::Mat_<char>(2,2) << -1, -1, -1, -1);
    cv::Mat ftKernel2 = (cv::Mat_<char>(2,2) << 1, 1, 1, 1);


    filter2D(pic1,fy1, CV_32FC1, fyKernel, cv::Point(0,0),0,cv::BORDER_REPLICATE);
    filter2D(pic2,fy2, CV_32FC1, fyKernel, cv::Point(0,0),0,cv::BORDER_REPLICATE);
    filter2D(pic1,fx1, CV_32FC1, fxKernel, cv::Point(0,0),0,cv::BORDER_REPLICATE);
    filter2D(pic2,fx2, CV_32FC1, fxKernel, cv::Point(0,0),0,cv::BORDER_REPLICATE);
    filter2D(pic1,ft1, CV_32FC1, ftKernel1, cv::Point(0,0),0,cv::BORDER_REPLICATE);
    filter2D(pic2,ft2, CV_32FC1, ftKernel2, cv::Point(0,0),0,cv::BORDER_REPLICATE);

    cv::Mat fx = (fx1 + fx2);
    cv::Mat fy = (fy1 + fy2);
    cv::Mat ft = (ft1 + ft2);

    cv::Mat u = cv::Mat::zeros(pic1.rows, pic1.cols, CV_32FC1);
    cv::Mat v = cv::Mat::zeros(pic1.rows, pic1.cols, CV_32FC1);

    cv::Mat weightedAverageKernel = (cv::Mat_<float>(3,3) << 1.0f/12.0f, 1.0f/6.0f, 1.0f/12.0f, 1.0f/6.0f, 0, 1.0f/6.0f, 1.0f/12.0f, 1.0f/6.0f, 1.0f/12.0f);
    cv::Mat uAverage;
    cv::Mat vAverage;

    for(int k = 0; k < iterations; k++)
    {
        filter2D(u,uAverage, -1, weightedAverageKernel, cv::Point(-1,-1),0,cv::BORDER_REPLICATE);
        filter2D(v,vAverage, -1, weightedAverageKernel, cv::Point(-1,-1),0,cv::BORDER_REPLICATE);

        for(int j = 0; j < pic1.rows; j++)
        {
            for(int i = 0; i < pic1.cols; i++)
            {
                u.at<float>(j,i) = uAverage.at<float>(j,i) - fx.at<float>(j,i) * (fx.at<float>(j,i) * uAverage.at<float>(j,i) + fy.at<float>(j,i) * vAverage.at<float>(j,i) + ft.at<float>(j,i))/(alpha*alpha + fx.at<float>(j,i)*fx.at<float>(j,i) + fy.at<float>(j,i)*fy.at<float>(j,i));
                v.at<float>(j,i) = vAverage.at<float>(j,i) - fy.at<float>(j,i) * (fx.at<float>(j,i) * uAverage.at<float>(j,i) + fy.at<float>(j,i) * vAverage.at<float>(j,i) + ft.at<float>(j,i))/(alpha*alpha + fx.at<float>(j,i)*fx.at<float>(j,i) + fy.at<float>(j,i)*fy.at<float>(j,i));
            }
        }
    }

    std::vector<cv::Mat> result;
    result.push_back(u);
    result.push_back(v);

    for (int j = 0; j < pic2.rows; j++)
    {
        for (int i = 0; i < pic2.cols; i++)
        {
            if((i*pic2.cols + j) % 100 == 0 && cv::Point(result[0].at<float>(j,i),result[1].at<float>(j,i) ) != cv::Point(0,0))
                drawArrow(pic3, cv::Point(i,j),cv::Point(i,j) + cv::Point(result[0].at<float>(j,i),result[1].at<float>(j,i)), cv::Scalar(0,0,255));
        }
    }
    return pic3;
}
