#ifndef IOPTICALFLOWALGORITHM_H
#define IOPTICALFLOWALGORITHM_H

#include "opencv2/highgui/highgui.hpp"

class IOpticalFlowAlgorithm
{
public:
    IOpticalFlowAlgorithm(){}
    virtual ~IOpticalFlowAlgorithm(){};

    virtual char* getName() = 0;
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage) = 0;
};

#endif // IOPTICALFLOWALGORITHM_H
