#pragma once
#include <opencv2/highgui/highgui.hpp>

class IOpticalFlowAlgorithm
{
public:
    virtual ~IOpticalFlowAlgorithm();

	// returns the current image with arrows on it
	virtual cv::Mat DrawArrows(const cv::Mat& last_image, const cv::Mat& current_image) = 0;
};