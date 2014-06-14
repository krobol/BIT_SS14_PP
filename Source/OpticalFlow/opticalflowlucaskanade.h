#ifndef OPTICALFLOWLUCASKANADE_H
#define OPTICALFLOWLUCASKANADE_H

#include "iopticalflowalgorithm.h"

class OpticalFlowLucasKanade : public IOpticalFlowAlgorithm
{
public:
    OpticalFlowLucasKanade();
    virtual char* getName();

    // Diese Funktion zeichnet die Pfeile auf das eingegebene Bild
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage);
};

#endif // OPTICALFLOWLUCASKANADE_H
