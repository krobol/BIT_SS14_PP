#ifndef OPTICALFLOWHORNSCHUNCK_H
#define OPTICALFLOWHORNSCHUNCK_H

#include "iopticalflowalgorithm.h"

class OpticalFlowHornSchunck : public IOpticalFlowAlgorithm
{
public:
    OpticalFlowHornSchunck();
    virtual char* getName();

    // Diese Funktion zeichnet die Pfeile auf das eingegebene Bild
    virtual cv::Mat drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage);
};

#endif // OPTICALFLOWHORNSCHUNCK_H
