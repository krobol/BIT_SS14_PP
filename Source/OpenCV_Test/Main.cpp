/*
Optical flow basically working but with a fatal flow right now. 
Every frame new features will be found, so basically no tracking happens.
Problem lies in the fact, that no further features will be found, 
after you got your first set of them.
TODO: Fix this.
TODO: Refactor for code conventions.
*/

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>

#define FEATURE_NUM 400

static const double pi = 3.14159265358979323846;

void drawArrow(cv::Mat image, cv::Point2f p, cv::Point2f q, cv::Scalar color, int arrowMagnitude = 8, int thickness=1, int line_type=8, int shift=0) 
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

int main(void)
{
	cv::Mat current_image, current_image_gray, next_image, next_image_gray, featuresMask;
	std::vector<cv::Point2f> features_prev, features_next;
	std::vector<uchar> optFlowStatus;
	std::vector<float> optFlowErr;
	cv::TermCriteria optFlowTermCrit = cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS),20, 0.3);
	cv::Size optFlowWinSize = cv::Size(10,10);

	
	long number_of_frames = 119;

	// Parameters of
	double qualityLevelFeatures = 0.4f;
	double minDistantceBetweenFeatures = 3.0f;
	int blockSizeFeature = 7;

	// Parameters of
	int maxLevelOpticalFlow = 5;
	double minEigThreshholdOpticalFlow = 0.05;

	unsigned int counter = 0;

	cv::VideoCapture sequence("G:/Users/Grigori/Desktop/E_training/training/1/0000000000.png");
	if (!sequence.isOpened())
    {
        std::cerr << "Failed to open image sequence (first image)!\n" << std::endl;
        return 1;
    }

	sequence >> next_image;

	if(next_image.empty())
	{
		return -1;
	}

	//featuresMask = (current_image.rows, current_image.cols, CV_BGR2GRAY);

	cv::cvtColor(next_image, next_image_gray, CV_BGR2GRAY);
	

	cv::goodFeaturesToTrack(next_image_gray, features_next, FEATURE_NUM, qualityLevelFeatures, minDistantceBetweenFeatures, cv::noArray(), blockSizeFeature, false, 0.04f);

	while (true)
	{
		counter++;
		features_prev = features_next;
		current_image = next_image.clone();
		cv::cvtColor(current_image, current_image_gray, CV_BGR2GRAY);
		sequence >> next_image; 
		cv::cvtColor(next_image, next_image_gray, CV_BGR2GRAY);

		if(next_image.empty())
        {
            std::cout << "End of Sequence." << std::endl;
            break;
        }

		if(features_prev.size() < (int)(FEATURE_NUM/2))
		{
			features_prev.clear();
			cv::goodFeaturesToTrack(current_image_gray, features_prev, FEATURE_NUM, qualityLevelFeatures, minDistantceBetweenFeatures, cv::noArray(), blockSizeFeature, false, 0.04f);
		}

		if(features_next.size() < (int)(FEATURE_NUM/2))
		{
			features_next.clear();
		}

		cv::goodFeaturesToTrack(next_image_gray, features_next, FEATURE_NUM, qualityLevelFeatures, minDistantceBetweenFeatures, cv::noArray(), blockSizeFeature, false, 0.04f);

		if(features_prev.empty() || features_next.empty())
		{
			std::cout << "Getting good features to track went wrong." << std::endl;
            break;
		}



		cv::calcOpticalFlowPyrLK(current_image_gray, next_image_gray, 
								features_prev, features_next,
								optFlowStatus, optFlowErr,
								optFlowWinSize,  maxLevelOpticalFlow,
								optFlowTermCrit, 0, 
								minEigThreshholdOpticalFlow);


		for (int i = 0; i < features_prev.size(); i++)
		{
			drawArrow(current_image, features_prev[i], features_next[i], cv::Scalar(0,0,255));
		}

		cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
		cv::imshow( "Display window", current_image );

		if(cv::waitKey(30) >= 0 || counter >= 119) 
		{
			break;
		}
	}
}