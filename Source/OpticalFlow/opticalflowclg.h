#ifndef OPTICALFLOWCLG_H
#define OPTICALFLOWCLG_H

#include "iopticalflowalgorithm.h"

class OpticalFlowCLG : public IOpticalFlowAlgorithm
{
public:
    OpticalFlowCLG();
    virtual char* getName();
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage);
};

#endif // OPTICALFLOWCLG_H
