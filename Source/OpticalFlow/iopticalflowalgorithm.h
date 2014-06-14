#ifndef IOPTICALFLOWALGORITHM_H
#define IOPTICALFLOWALGORITHM_H

#include <iostream>

#include "AlgorithmConfig.h"
#include <memory>

#include "opencv/cv.h"

static const double pi = 3.14159265358979323846;

// Desired number of features to trac by the optical flow algorithm.
#define FEATURE_NUM 200

namespace cv
{
    class Mat;
}

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

typedef std::shared_ptr<IOpticalFlowAlgorithm> OpticalFlowAlgorithmPtr;

inline void drawArrow(cv::Mat image, cv::Point2f p, cv::Point2f q, cv::Scalar color = cv::Scalar(255,255,255), int arrowMagnitude = 3, int thickness=1, int line_type=8, int shift=0)
{
    //Draw the principle line
    cv::line(image, p, q, color, thickness, line_type, shift);


    //compute the angle alpha
    double angle = atan2((double)p.y-q.y, (double)p.x-q.x);
    //compute the coordinates of the first segment
    p.x = ( q.x + (float) (arrowMagnitude * cos(angle + pi/4.0)));
    p.y = ( q.y + (float) (arrowMagnitude * sin(angle + pi/4.0)));

    //Draw the first segment
    cv::line(image, p, q, color, thickness, line_type, shift);
    //compute the coordinates of the second segment
    p.x = ( q.x +  (float) (arrowMagnitude * cos(angle - pi/4.0)));
    p.y = ( q.y +  (float) (arrowMagnitude * sin(angle - pi/4.0)));
    //Draw the second segment
    cv::line(image, p, q, color, thickness, line_type, shift);

}

#endif // IOPTICALFLOWALGORITHM_H
