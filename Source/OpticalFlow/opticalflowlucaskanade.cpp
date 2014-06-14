#include "opticalflowlucaskanade.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>

OpticalFlowLucasKanade::OpticalFlowLucasKanade()
{
    // Define Options
    config.addParameter("quadOptWinSize", ConfigValueDescription("quadOptWinSize", 0, 1, 40)); // Beschreibung der Option festlegen
    config.setValue("quadOptWinSize", 20); // Optionswert lesen
}

char* OpticalFlowLucasKanade::getName()
{
    return "LucasKanade";
}

// 2. Compute u and v by using the sum of derivatives in a special formula resulting from the least squared fit on u and v.
// Make this function maybe return a boolean, in case the algorithm is supposed to work iteratively, until some error measurement is satisfied.
void ComputeUV(std::vector<cv::Mat>& vec_uv,
               cv::Mat derivative_f_x,
               cv::Mat derivative_f_y,
               cv::Mat derivative_f_t,
               const unsigned short quadOptWinSize)
{
    //std::cout << "M = " << std::endl << " " << derivative_f_x << std::endl << std::endl;
    //std::cout << "M = " << std::endl << " " << derivative_f_y << std::endl << std::endl;
    //std::cout << "M = " << std::endl << " " << derivative_f_t << std::endl << std::endl;

    cv::Mat fx_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    fx_squared = derivative_f_x.mul(derivative_f_x);

    cv::Mat fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    fy_squared = derivative_f_y.mul(derivative_f_y);

    cv::Mat fx_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    fx_ft = derivative_f_x.mul(derivative_f_t);

    cv::Mat fx_fy = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    fx_fy = derivative_f_x.mul(derivative_f_y);

    cv::Mat fy_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    fy_ft = derivative_f_y.mul(derivative_f_t);

    /*
    std::cout << "fx_squared = " << std::endl << " " << fx_squared << std::endl << std::endl;
    std::cout << "fy_squared = " << std::endl << " " << fy_squared << std::endl << std::endl;
    std::cout << "fx_ft = " << std::endl << " " << fx_ft << std::endl << std::endl;
    std::cout << "fx_fy = " << std::endl << " " << fx_fy << std::endl << std::endl;
    std::cout << "fy_ft = " << std::endl << " " << fy_ft << std::endl << std::endl;
    */

    cv::Mat sum_fx_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_fy = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fy_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    cv::Mat sum_fx_squared_sum_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    cv::Mat sum_Kernel = cv::Mat::ones(quadOptWinSize,quadOptWinSize,CV_32FC1);	//(cv::Mat_<char>(10,10) << 1, 1, 1, 1, 1, 1, 1, 1, 1);

    /*
    cv::filter2D(fx_squared,sum_fx_squared, CV_32FC1, sum_Kernel, cv::Point(1,1), 0, cv::BORDER_REPLICATE);
    cv::filter2D(fy_squared,sum_fy_squared, CV_32FC1, sum_Kernel, cv::Point(1,1), 0, cv::BORDER_REPLICATE);
    cv::filter2D(fx_ft,sum_fx_ft, CV_32FC1, sum_Kernel, cv::Point(1,1), 0, cv::BORDER_REPLICATE);
    cv::filter2D(fx_fy,sum_fx_fy, CV_32FC1, sum_Kernel, cv::Point(1,1), 0, cv::BORDER_REPLICATE);
    cv::filter2D(fy_ft,sum_fy_ft, CV_32FC1, sum_Kernel, cv::Point(1,1), 0, cv::BORDER_REPLICATE);
    */

    cv::filter2D(fx_squared,sum_fx_squared, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fy_squared,sum_fy_squared, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fx_ft,sum_fx_ft, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fx_fy,sum_fx_fy, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fy_ft,sum_fy_ft, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);

    /*
    std::cout << "sum_fx_squared = " << std::endl << " " << sum_fx_squared << std::endl << std::endl;
    std::cout << "sum_fy_squared = " << std::endl << " " << sum_fy_squared << std::endl << std::endl;
    std::cout << "sum_fx_ft = " << std::endl << " " << sum_fx_ft << std::endl << std::endl;
    std::cout << "sum_fx_fy = " << std::endl << " " << sum_fx_fy << std::endl << std::endl;
    std::cout << "sum_fy_ft = " << std::endl << " " << sum_fy_ft << std::endl << std::endl;
    */

    cv::Mat u = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat v = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    sum_fx_squared_sum_fy_squared = sum_fx_squared.mul(sum_fy_squared);
    sum_fx_fy_squared = sum_fx_fy.mul(sum_fx_fy);

    /*
    std::cout << "sum_fx_squared_sum_fy_squared = " << std::endl << " " << sum_fx_squared_sum_fy_squared << std::endl << std::endl;
    std::cout << "sum_fx_fy_squared = " << std::endl << " " << sum_fx_fy_squared << std::endl << std::endl;
    */

    for (int i = 0; i < derivative_f_x.cols; i++)
    {
        for (int j = 0; j < derivative_f_x.rows; j++)
        {
            float denominator_uv = sum_fx_squared_sum_fy_squared.at<float>(j,i) - sum_fx_fy_squared.at<float>(j,i);
            if(denominator_uv != 0)
            {
                u.at<float>(j,i) = ((-sum_fy_squared.at<float>(j,i)*sum_fx_ft.at<float>(j,i))+(sum_fx_fy.at<float>(j,i)*sum_fy_ft.at<float>(j,i)))/denominator_uv;
                v.at<float>(j,i) = ((sum_fx_ft.at<float>(j,i)*sum_fx_fy.at<float>(j,i))-(sum_fx_squared.at<float>(j,i)*sum_fy_ft.at<float>(j,i)))/denominator_uv;
            }
            else
            {
                u.at<float>(j,i) = 0;
                v.at<float>(j,i) = 0;
            }
        }
    }

    /*
    std::cout << "U = " << std::endl << " " << u << std::endl << std::endl;
    std::cout << "V = " << std::endl << " " << v << std::endl << std::endl;
    */

    vec_uv.push_back(u);
    vec_uv.push_back(v);
}

// 1. Compute derivatives in x, y and t direction for whole picture, using a small quadratical window for each pixel.
void ComputeDerivativesAndUV(cv::Mat previous_img, cv::Mat current_img,
                             std::vector<cv::Mat>& vec_uv,
                             const unsigned short quadOptWinSize)
{
    cv::Mat f_x1;
    cv::Mat f_y1;
    cv::Mat f_t1;

    cv::Mat f_x2;
    cv::Mat f_y2;
    cv::Mat f_t2;

    cv::Mat f_x_Kernel =			(cv::Mat_<char>(2,2) << -1, 1, -1, 1);
    cv::Mat f_y_Kernel =			(cv::Mat_<char>(2,2) << -1, -1, 1, 1);
    cv::Mat f_t_previous_Kernel =	(cv::Mat_<char>(2,2) << -1, -1, -1, -1);
    cv::Mat f_t_current_Kernel =	(cv::Mat_<char>(2,2) << 1, 1, 1, 1);

    cv::filter2D(previous_img,f_x1, CV_32FC1, f_x_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img, f_x2, CV_32FC1, f_x_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(previous_img,f_y1, CV_32FC1, f_y_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img, f_y2, CV_32FC1, f_y_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(previous_img,f_t1, CV_32FC1, f_t_previous_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img, f_t2, CV_32FC1, f_t_current_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);

    cv::Mat f_x = (f_x1+f_x2);
    cv::Mat f_y = (f_y1+f_y2);
    cv::Mat f_t = (f_t1+f_t2);

    // Use the method to compute u and v and write results into 'img_uv'.
    ComputeUV(vec_uv, f_x, f_y, f_t, quadOptWinSize);
}

cv::Mat OpticalFlowLucasKanade::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    int quadOptWinSize = config.getValue("quadOptWinSize"); // Optionswert lesen

    cv::Mat pic1, pic2, pic3;
    cv::cvtColor(lastImage,     pic1, CV_BGR2GRAY);
    cv::cvtColor(currentImage,  pic2, CV_BGR2GRAY);
    currentImage.copyTo(pic3);

    std::vector<cv::Mat> result;
    ComputeDerivativesAndUV(pic1, pic2, result, quadOptWinSize);

    for (int j = 0; j < pic2.rows; j++)
    {
        for (int i = 0; i < pic2.cols; i++)
        {
            if((i*pic2.cols + j) % 100 == 0 && cv::Point(result[0].at<float>(j,i),result[1].at<float>(j,i) ) != cv::Point(0,0))
                drawArrow(pic3, cv::Point(i,j),cv::Point(i,j) + cv::Point(result[0].at<float>(j,i),result[1].at<float>(j,i)));
        }
    }
    return pic3;
}
