#include "opticalflowhornschunck.h"

OpticalFlowHornSchunck::OpticalFlowHornSchunck()
{
    // Define Options
    config.addParameter("Iterations", ConfigValueDescription("Iterations", 0, 0, 500)); // Beschreibung der Option festlegen
    config.setValue("Iterations", 5);   // Default wert einstellen

    config.addParameter("Alpha", ConfigValueDescription("Alpha", 2, 0, 1)); // Beschreibung der Option festlegen
    config.setValue("Alpha", 0.5);
}

char* OpticalFlowHornSchunck::getName()
{
    return "Horn–Schunck";
}

cv::Mat OpticalFlowHornSchunck::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    int iterations = config.getValue("Iterations"); // Optionswert lesen
    return currentImage;
}
