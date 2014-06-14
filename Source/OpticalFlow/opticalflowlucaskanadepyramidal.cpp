#include "opticalflowlucaskanadepyramidal.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>

OpticalFlowLucasKanadePyramidal::OpticalFlowLucasKanadePyramidal()
{
    // Define Options
    config.addParameter("quadOptWinSize", ConfigValueDescription("Kernel size", 0, 1, 40)); // Beschreibung der Option festlegen
    config.setValue("quadOptWinSize", 10);

    config.addParameter("level", ConfigValueDescription("Number of Levels", 0, 1, 8)); // Beschreibung der Option festlegen
    config.setValue("level", 2);
}

char* OpticalFlowLucasKanadePyramidal::getName()
{
    return "LucasKanade Multiresolution";
}

// Pyramidal Lucas-Kanade
//_______________________________________________________________________________________________________________________________________________

void BuildPyrramids(cv::Mat previous_img, cv::Mat current_img, std::vector<cv::Mat>& previous_img_pyramid, std::vector<cv::Mat>& current_img_pyramid, const unsigned short level)
{
    float scale = std::pow(0.5f,(float)level);

    cv::Mat previous_img_downsampled_tmp;
    cv::Mat current_img_downsampled_tmp;

    if(level != 0)
    {
        for (float i = 0; i < level-1; i++)
        {
            scale *= 2.0f;
            cv::resize(previous_img,previous_img_downsampled_tmp,cv::Size(),scale,scale,CV_INTER_AREA);
            cv::resize(current_img,current_img_downsampled_tmp,cv::Size(),scale,scale,CV_INTER_AREA);

            previous_img_pyramid.push_back(previous_img_downsampled_tmp);
            current_img_pyramid.push_back(current_img_downsampled_tmp);
        }
    }

    previous_img_pyramid.push_back(previous_img);
    current_img_pyramid.push_back(current_img);
}

void ComputeUV_(std::vector<cv::Mat>& vec_uv,
               cv::Mat derivative_f_x,
               cv::Mat derivative_f_y,
               cv::Mat derivative_f_t,
               const unsigned short quadOptWinSize)
{
    vec_uv.clear();

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

    cv::Mat sum_fx_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_fy = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fy_ft = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    cv::Mat sum_fx_squared_sum_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat sum_fx_fy_squared = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    cv::Mat sum_Kernel = cv::Mat::ones(quadOptWinSize,quadOptWinSize,CV_32FC1);	//(cv::Mat_<char>(10,10) << 1, 1, 1, 1, 1, 1, 1, 1, 1);

    cv::filter2D(fx_squared,sum_fx_squared, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fy_squared,sum_fy_squared, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fx_ft,sum_fx_ft, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fx_fy,sum_fx_fy, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);
    cv::filter2D(fy_ft,sum_fy_ft, CV_32FC1, sum_Kernel, cv::Point(quadOptWinSize/2,quadOptWinSize/2), 0, cv::BORDER_DEFAULT);

    cv::Mat u = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);
    cv::Mat v = cv::Mat::zeros(derivative_f_x.rows,derivative_f_x.cols, CV_32FC1);

    sum_fx_squared_sum_fy_squared = sum_fx_squared.mul(sum_fy_squared);
    sum_fx_fy_squared = sum_fx_fy.mul(sum_fx_fy);

    for (int i = 0; i < derivative_f_x.cols; i++)
    {
        for (int j = 0; j < derivative_f_x.rows; j++)
        {
            float denominator_uv = sum_fx_squared_sum_fy_squared.at<float>(j,i) - sum_fx_fy_squared.at<float>(j,i);
            if(denominator_uv == 0.0f)
            {
                u.at<float>(j,i) = 0.0f;
                v.at<float>(j,i) = 0.0f;
            }
            else
            {
                u.at<float>(j,i) = ((-sum_fy_squared.at<float>(j,i)*sum_fx_ft.at<float>(j,i))+(sum_fx_fy.at<float>(j,i)*sum_fy_ft.at<float>(j,i)))/denominator_uv;
                v.at<float>(j,i) = ((sum_fx_ft.at<float>(j,i)*sum_fx_fy.at<float>(j,i))-(sum_fx_squared.at<float>(j,i)*sum_fy_ft.at<float>(j,i)))/denominator_uv;
            }
        }
    }

    vec_uv.push_back(u);
    vec_uv.push_back(v);
}

void ComputeUVPyrramidal(std::vector<cv::Mat>& vec_uv,
                        cv::Mat derivative_f_x,
                        cv::Mat derivative_f_y,
                        cv::Mat derivative_f_t,
                        const unsigned short quadOptWinSize)
{
    if(vec_uv.empty())
    {
        ComputeUV_(vec_uv, derivative_f_x,derivative_f_y, derivative_f_t, quadOptWinSize);
    }
    else
    {
        std::vector<cv::Mat> previous_vec_uv(vec_uv);

        // Run ComputeUV(...) again to refine our results.
        ComputeUV_(vec_uv, derivative_f_x,derivative_f_y, derivative_f_t, quadOptWinSize);

        // Add newly computed UV vectors to the previous UV vectors to refine the result
        vec_uv[0] += previous_vec_uv[0];
        vec_uv[1] += previous_vec_uv[1];
    }
}

// TODO: Test this method.
void ComputeDerivativesAndUVPyrramidal(cv::Mat previous_img_pyrramid,
                                    cv::Mat current_img_pyrramid,
                                    std::vector<cv::Mat>& vec_uv,
                                    const unsigned short quadOptWinSize,
                                    const unsigned short run)
{
    cv::Mat img_pyrramid_derivative_x;
    cv::Mat img_pyrramid_derivative_y;
    cv::Mat img_pyrramid_derivative_t;

    cv::Mat f_x_Kernel =			(cv::Mat_<char>(2,2) << -1, 1, -1, 1);
    cv::Mat f_y_Kernel =			(cv::Mat_<char>(2,2) << -1, -1, 1, 1);
    cv::Mat f_t_previous_Kernel =	(cv::Mat_<char>(2,2) << -1, -1, -1, -1);
    cv::Mat f_t_current_Kernel =	(cv::Mat_<char>(2,2) << 1, 1, 1, 1);

    cv::Mat f_x1;
    cv::Mat f_y1;
    cv::Mat f_t1;

    cv::Mat f_x2;
    cv::Mat f_y2;
    cv::Mat f_t2;

    cv::filter2D(previous_img_pyrramid,f_x1, CV_32FC1, f_x_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img_pyrramid, f_x2, CV_32FC1, f_x_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(previous_img_pyrramid,f_y1, CV_32FC1, f_y_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img_pyrramid, f_y2, CV_32FC1, f_y_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(previous_img_pyrramid,f_t1, CV_32FC1, f_t_previous_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);
    cv::filter2D(current_img_pyrramid, f_t2, CV_32FC1, f_t_current_Kernel, cv::Point(0,0), 0, cv::BORDER_DEFAULT);

    cv::Mat f_x = (f_x1+f_x2);
    cv::Mat f_y = (f_y1+f_y2);
    cv::Mat f_t;

    if(vec_uv.empty())
    {
        f_t = (f_t1+f_t2);
    }
    else
    {
        vec_uv[0] *= 2.0f;
        vec_uv[1] *= 2.0f;
        // Bicubic interpolation of missing previous UV vectors, by up scaling through resizing with OpenCV.
        cv::resize(vec_uv[0],vec_uv[0],previous_img_pyrramid.size(),0,0, CV_INTER_CUBIC);
        cv::resize(vec_uv[1],vec_uv[1],previous_img_pyrramid.size(),0,0, CV_INTER_CUBIC);

        f_t = f_t1.clone();
        for (int i = 0; i < current_img_pyrramid.cols; i++)
        {
            for (int j = 0; j < current_img_pyrramid.rows; j++)
            {
                int index_x_u = (i+(int)vec_uv[0].at<float>(j,i));
                int index_y_v = (j+(int)vec_uv[1].at<float>(j,i));

                // Border case handling.
                if(index_x_u >= current_img_pyrramid.cols)
                    index_x_u = current_img_pyrramid.cols-1;
                if(index_x_u < 0)
                    index_x_u = 0;
                if(index_y_v >= current_img_pyrramid.rows)
                    index_y_v = current_img_pyrramid.rows-1;
                if(index_y_v < 0)
                    index_y_v = 0;

                f_t.at<float>(j,i)+=f_t2.at<float>(index_y_v,index_x_u);
            }
        }
    }

    // Use the method to compute u and v and write results into 'img_uv'.
    ComputeUVPyrramidal(vec_uv, f_x, f_y, f_t, quadOptWinSize);
}

cv::Mat OpticalFlowLucasKanadePyramidal::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    const unsigned short quadOptWinSize = config.getValue("quadOptWinSize"); // Optionswert lesen
    const unsigned short level = config.getValue("level"); // Optionswert lesen

    cv::Mat pic1, pic2, pic3;
    cv::cvtColor(lastImage,     pic1, CV_BGR2GRAY);
    cv::cvtColor(currentImage,  pic2, CV_BGR2GRAY);
    currentImage.copyTo(pic3);

    std::vector<cv::Mat> result;

    std::vector<cv::Mat> previous_img_pyramid;
    std::vector<cv::Mat> current_img_pyramid;

    BuildPyrramids(pic1, pic2, previous_img_pyramid, current_img_pyramid, level);

    for (int i = 0; i < level; i++)
    {
        ComputeDerivativesAndUVPyrramidal(previous_img_pyramid[i],current_img_pyramid[i], result, quadOptWinSize, i);
    }

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
