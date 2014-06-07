/*
Optical flow basically working but with a fatal flow right now. 
Every frame new features will be found, so basically no tracking happens.
Problem lies in the fact, that no further features will be found, 
after you got your first set of them.
TODO: Fix this (Kind of works now).
*/

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <opencv2\highgui\highgui.hpp>

// Desired number of features to trac by the optical flow algorithm.
#define FEATURE_NUM 200

static const double pi = 3.14159265358979323846;

// Helper function for drawing arrows.
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
	// Current and next Images of an image sequence stored as 'Mat'.
	cv::Mat current_image, current_image_gray, next_image, next_image_gray;
	// Features of current and next image to be found and matched by the optical flow algorithm 'calcOpticalFlowPyrLK(...)'.
	std::vector<cv::Point2f> current_features, next_features;

	short actual_feature_num = 0;

	// Parameters of 'goodFeaturesToTrack(...)'
	double quality_level_features = 0.5f;
	double min_distantce_between_features = 3.0f;
	int block_size_feature = 4;

	// Parameters of 'calcOpticalFlowPyrLK(...)'
	std::vector<uchar> opt_flow_status;
	std::vector<float> opt_flow_error;
	cv::TermCriteria opt_flow_term_crit = 
		cv::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 20, 0.5);
	cv::Size opt_flow_win_size = cv::Size(20,20);
	int flag = 0;

	//optional: CV_LKFLOW_INITIAL_GUESSES or CV_LKFLOW_GET_MIN_EIGENVALS
	int max_level_optical_flow = 10;
	double min_eig_threshhold_optical_flow = 0.01;

	unsigned int counter = 0;

	// Initializing the image sequence.  
	cv::VideoCapture sequence("G:/Users/Grigori/Desktop/E_training/training/1/0000000000.png");
	if (!sequence.isOpened())
    {
        std::cerr << "Failed to open image sequence (first image)!\n" << std::endl;
        return -1;
    }

	// Getting current image.
	sequence >> next_image;

	// Checking if first image is empty, which should not happen.
	if(next_image.empty())
	{
		std::cerr << "Failed to load first image!\n" << std::endl;
		return -1;
	}

	// Converting image format.
	cv::cvtColor(next_image, next_image_gray, CV_BGR2GRAY);
	
	// Using Shi-Tomasi corner detector to find good features to track.
	cv::goodFeaturesToTrack(next_image_gray, next_features, FEATURE_NUM, quality_level_features, min_distantce_between_features, cv::noArray(), block_size_feature, false, 0.04f);

	actual_feature_num = next_features.size();

	// Create a window for display.
	cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );				

	// Trackbar test.___________________________________________________________________________________________________________________________
	cv::createTrackbar("Test Maximum Optical Flow Level","Display window", &max_level_optical_flow,20);
	//__________________________________________________________________________________________________________________________________________

	while (true)
	{
		// Updating images.
		current_features = next_features;
		current_image = next_image.clone();
		// Converting image format to gray scale.
		cv::cvtColor(current_image, current_image_gray, CV_BGR2GRAY);
		sequence >> next_image; 
		// Converting image format to gray scale.
		cv::cvtColor(next_image, next_image_gray, CV_BGR2GRAY);

		// Assuming that image sequence is at its end, if an empty image occurs.
		if(next_image.empty())
        {
            std::cout << "End of Sequence." << std::endl;
            break;
        }

		// Making sure, features that if too many features vanish new will be found to track.
		if(current_features.size() < actual_feature_num/2)
		{
			current_features.clear();
			cv::goodFeaturesToTrack(current_image_gray, current_features, FEATURE_NUM, quality_level_features, min_distantce_between_features, cv::noArray(), block_size_feature, false, 0.04f);
			// Adapting feature count number, to accordingly estimate, when our tracked features should be reefreshed.
			actual_feature_num += current_features.size();
			actual_feature_num /= 2;
		}
		if(next_features.size() < actual_feature_num/2)
		{
			next_features.clear();
			cv::goodFeaturesToTrack(next_image_gray, next_features, FEATURE_NUM, quality_level_features, min_distantce_between_features, cv::noArray(), block_size_feature, false, 0.04f);
			// Adapting feature count number, to accordingly estimate, when our tracked features should be reefreshed.
			actual_feature_num += next_features.size();
			actual_feature_num /= 2;
		}

		if(current_features.empty() || next_features.empty())
		{
			std::cerr << "Getting good features to track went wrong." << std::endl;
            return -1;
		}

		// CV's optical flow algorithm. Requires images to be in gray scale.
		cv::calcOpticalFlowPyrLK(current_image_gray, next_image_gray, 
								current_features, next_features,
								opt_flow_status, opt_flow_error,
								opt_flow_win_size,  max_level_optical_flow,
								opt_flow_term_crit, flag, 
								min_eig_threshhold_optical_flow);

		// Drawing arrows here.
		for (int i = 0; i < current_features.size(); i++)
		{
			// Features, that ar not moving will be deleted and therefore not be drawn as arrows.
			if(next_features[i].x-current_features[i].x < 0.3f && next_features[i].y-current_features[i].y < 0.3f)
			{
				current_features.erase(current_features.begin() + i);
				next_features.erase(next_features.begin() + i);
			}
			else
			{
				drawArrow(current_image, current_features[i], next_features[i], cv::Scalar(0,0,255));
			}
		}

		cv::imshow( "Display window", current_image );

		// Some debug stuff.
		//std::cout << "Number of features: " << features_next.size() << std::endl;
		std::cout << "actual_feature_num: " << actual_feature_num << std::endl;
		//std::cout << "max_level_optical_flow: " << max_level_optical_flow << std::endl;

		// Waiter function to ensure, that 'imshow(...)' has enough time to draw and display the image.
		if(cv::waitKey(30) >= 0) 
		{
			break;
		}
	}
	// Free ressources.
	cv::destroyAllWindows();
}