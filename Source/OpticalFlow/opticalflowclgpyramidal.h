#ifndef OPTICALFLOWCLGPYRAMIDAL_H
#define OPTICALFLOWCLGPYRAMIDAL_H

#include "iopticalflowalgorithm.h"

class OpticalFlowClgPyramidal : public IOpticalFlowAlgorithm
{
public:
    OpticalFlowClgPyramidal();
    virtual char* getName();
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage);
};

#endif // OPTICALFLOWCLGPYRAMIDAL_H
