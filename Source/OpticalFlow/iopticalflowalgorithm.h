#ifndef IOPTICALFLOWALGORITHM_H
#define IOPTICALFLOWALGORITHM_H

#include "opencv2/highgui/highgui.hpp"

#include <iostream>

#include "AlgorithmConfig.h"

class IOpticalFlowAlgorithm
{
public:
    IOpticalFlowAlgorithm() : config()
    {}
    virtual ~IOpticalFlowAlgorithm(){};

    virtual char* getName() = 0;
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage) = 0;


    AlgorithmConfig* getConfig()
    {
        return &config;
    }

protected:
    AlgorithmConfig config;
};

#endif // IOPTICALFLOWALGORITHM_H
