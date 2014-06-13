#include "opticalflowhornschunck.h"

OpticalFlowHornSchunck::OpticalFlowHornSchunck()
{
}

char* OpticalFlowHornSchunck::getName()
{
    return "Horn–Schunck";
}

cv::Mat OpticalFlowHornSchunck::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    return currentImage;
}
