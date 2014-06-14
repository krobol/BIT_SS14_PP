#ifndef OPTICALFLOWLUCASKANADEPYRAMIDAL_H
#define OPTICALFLOWLUCASKANADEPYRAMIDAL_H

#include "iopticalflowalgorithm.h"

class OpticalFlowLucasKanadePyramidal : public IOpticalFlowAlgorithm
{
public:
    OpticalFlowLucasKanadePyramidal();
    virtual char* getName();
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage);
};

#endif // OPTICALFLOWLUCASKANADEPYRAMIDAL_H
