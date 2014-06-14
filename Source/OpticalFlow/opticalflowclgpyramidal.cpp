#include "opticalflowclgpyramidal.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <math.h>

OpticalFlowClgPyramidal::OpticalFlowClgPyramidal()
{
    // Define Options
    config.addParameter("iterations", ConfigValueDescription("Iterations", 0, 1, 10)); // Beschreibung der Option festlegen
    config.setValue("iterations", 1);   // Default wert einstellen

    config.addParameter("alpha", ConfigValueDescription("Global Smoothing", 2, 0.1f, 10.0f)); // Beschreibung der Option festlegen
    config.setValue("alpha", 2.0f);

    config.addParameter("rho", ConfigValueDescription("Local spatio-temporal smoothing", 2, 0, 2)); // Beschreibung der Option festlegen
    config.setValue("rho", 1.0f);

    config.addParameter("sigma", ConfigValueDescription("Standard deviation", 2, 3, 5)); // Beschreibung der Option festlegen
    config.setValue("sigma", 3.0f);

    config.addParameter("w", ConfigValueDescription("SOR Relaxation", 2, 0, 2)); // Beschreibung der Option festlegen
    config.setValue("w", 0.2f);

    config.addParameter("nScales", ConfigValueDescription("Number of Scales", 0, 1, 6)); // Beschreibung der Option festlegen
    config.setValue("nScales", 4.0f);

    config.addParameter("scaleFactor", ConfigValueDescription("Downsampling factor", 2, 0.5f, 1.0f)); // Beschreibung der Option festlegen
    config.setValue("scaleFactor", 0.65f);
}

char* OpticalFlowClgPyramidal::getName()
{
    return "Combined Local Global Pyramidal";
}

std::vector<cv::Mat> CLG(cv::Mat pic1, cv::Mat pic2, int iterations, float alpha, float w, float sigma, cv::Mat uOut, cv::Mat vOut)
{
    //compute derivates according to
    cv::Mat fy = cv::Mat::zeros(pic1.rows, pic1.cols, CV_32FC1);
    cv::Mat fx = cv::Mat::zeros(pic1.rows, pic1.cols, CV_32FC1);;
    cv::Mat ft = pic1-pic2;
    ft.convertTo(ft, CV_32FC1);
    cv::Mat fyKernel = (cv::Mat_<float>(5,1) << -1, 8, 0, -8, 1)/12;
    cv::Mat fxKernel = (cv::Mat_<char>(1,5) << -1, 8, 0, -8, 1)/12;

    filter2D(pic1,fy, CV_32FC1, fyKernel, cv::Point(0, 2),0,cv::BORDER_REPLICATE);
    filter2D(pic1,fx, CV_32FC1, fxKernel, cv::Point(2,0),0,cv::BORDER_REPLICATE);

    //std::cout << "fx = " << std::endl << " " << fx << std::endl << std::endl;
    cv::Mat u = uOut;
    cv::Mat v = vOut;

    float h = 1.0f;
    cv::Mat fxSquared = fx.mul(fx);
    cv::Mat fySquared = fy.mul(fy);

    cv::Mat fxfy = fx.mul(fy);
    cv::Mat fxft = fx.mul(ft);
    cv::Mat fyft = fy.mul(ft);

    if(sigma < 3) //minimum size
        sigma = 3;

    int size = 2*(2*sigma+1)+1;
    cv::Mat gaussianKernelY = cv::getGaussianKernel(size, sigma, CV_64F);
    cv::Mat gaussianKernelX;
    transpose(gaussianKernelY, gaussianKernelX);

    //normalize
    double norm = 0;
    for(int i = (gaussianKernelX.cols+1)/2; i < gaussianKernelX.cols; i++)
        norm += gaussianKernelX.at<double>(0,i);
    norm *= 2;
    norm -= gaussianKernelX.at<double>(0,(gaussianKernelX.cols+1)/2);
    gaussianKernelY /= norm;
    gaussianKernelX /= norm;

    filter2D(fxSquared,fxSquared,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(fxSquared,fxSquared,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);
    filter2D(fySquared,fySquared,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(fySquared,fySquared,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);
    filter2D(fxfy,fxfy,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(fxfy,fxfy,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);
    filter2D(fxft,fxft,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(fxft,fxft,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);
    filter2D(fyft,fyft,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(fyft,fyft,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);


    for(int k = 0; k < iterations; k++)
    {
        for(int j = 0; j < pic1.rows; j++)
        {
            for(int i = 0; i < pic1.cols; i++)
            {
                float sumNU = 0.0f; // sum of N positiv and N negativ for u
                float sumNV = 0.0f; // sum of N positiv and N negativ for v
                int counter = 0;
                if(i > 0)
                {
                    sumNU += u.at<float>(j, i-1);
                    sumNV += v.at<float>(j, i-1);
                    counter++;
                }

                if(i < u.cols-1)
                {
                    sumNU += u.at<float>(j, i+1);
                    sumNV += v.at<float>(j, i+1);
                    counter++;
                }

                if(j > 0)
                {
                    sumNU += u.at<float>(j-1, i);
                    sumNV += v.at<float>(j-1, i);
                    counter++;
                }

                if(j < u.rows-1)
                {
                    sumNU += u.at<float>(j+1, i);
                    sumNV += v.at<float>(j+1, i);
                    counter++;
                }

                if(i > 0 && j > 0)
                {
                    sumNU += u.at<float>(j-1, i-1);
                    sumNV += v.at<float>(j-1, i-1);
                    counter++;
                }

                if(i < u.cols-1 && j > 0)
                {
                    sumNU += u.at<float>(j-1, i+1);
                    sumNV += v.at<float>(j-1, i+1);
                    counter++;
                }

                if(i > 0 && j < u.rows-1)
                {
                    sumNU += u.at<float>(j+1, i-1);
                    sumNV += v.at<float>(j+1, i-1);
                    counter++;
                }

                if(i < u.cols-1 && j < u.rows-1)
                {
                    sumNU += u.at<float>(j+1, i+1);
                    sumNV += v.at<float>(j+1, i+1);
                    counter++;
                }
                u.at<float>(j,i) = (1-w)*u.at<float>(j,i) + w * (sumNU - (h*h/alpha)*(fxfy.at<float>(j,i)*v.at<float>(j,i)+fxft.at<float>(j,i)))/(counter + (h*h/alpha)*fxSquared.at<float>(j,i));
                v.at<float>(j,i) = (1-w)*v.at<float>(j,i) + w * (sumNV - (h*h/alpha)*(fxfy.at<float>(j,i)*u.at<float>(j,i)+fyft.at<float>(j,i)))/(counter + (h*h/alpha)*fySquared.at<float>(j,i));
            }
        }
    }

    std::vector<cv::Mat> result;
    result.push_back(u);
    result.push_back(v);
    return result;
}

cv::Mat OpticalFlowClgPyramidal::drawArrows(const cv::Mat& lastImage, const cv::Mat& currentImage)
{
    cv::Mat pic1, pic2, pic3;
    cv::cvtColor(lastImage,     pic1, CV_BGR2GRAY);
    cv::cvtColor(currentImage,  pic2, CV_BGR2GRAY);
    currentImage.copyTo(pic3);

    int iterations = config.getValue("iterations");
    float alpha = config.getValue("alpha");
    float rho = config.getValue("rho");
    float sigma = config.getValue("sigma");
    float w = config.getValue("w");
    int nScales = config.getValue("nScales");
    float scaleFactor = config.getValue("scaleFactor");

    std::vector<int> nx; // contains the width of the different scales
    std::vector<int> ny; // contains the height of the different scales
    std::vector<cv::Mat> pic1Scales; // contains the different Scales of the first image
    std::vector<cv::Mat> pic2Scales; // contains the different Scales of the first image
    std::vector<cv::Mat> nu; // contains the horizontal solution of the Solution for each Scale
    std::vector<cv::Mat> nv; // contains the vertical component of the Solution for each Scale

    nx.push_back(pic1.cols);
    ny.push_back(pic1.rows);
    pic1Scales.push_back(pic1); // first scale is original picture
    pic2Scales.push_back(pic2); // first scale is original picture

    //calculate the width and height for each scale
    for(int i = 1; i < nScales; i++)
    {
        nx.push_back((int)((double) nx[i-1] * scaleFactor + 0.5));
        ny.push_back((int)((double) ny[i-1] * scaleFactor + 0.5));
    }

    //initialize nu and nv
    for(int i = 0; i < nScales; i++)
    {
        nu.push_back(cv::Mat::zeros(ny[i],nx[i],CV_32F));
        nv.push_back(cv::Mat::zeros(ny[i],nx[i],CV_32F));
    }

    //create gauss filter for pre-smoothing
    int size = 2*(2*sigma+1)+1;
    cv::Mat gaussianKernelY = cv::getGaussianKernel(size, sigma, CV_64F);
    cv::Mat gaussianKernelX;
    transpose(gaussianKernelY, gaussianKernelX);

    //normalize
    double norm = 0;
    for(int i = (gaussianKernelX.cols+1)/2; i < gaussianKernelX.cols; i++)
        norm += gaussianKernelX.at<double>(0,i);
    norm *= 2;
    norm -= gaussianKernelX.at<double>(0,(gaussianKernelX.cols+1)/2);
    gaussianKernelY /= norm;
    gaussianKernelX /= norm;

    filter2D(pic1,pic1,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(pic1,pic1,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);
    filter2D(pic2,pic2,-1,gaussianKernelX,cv::Point((gaussianKernelX.cols+1)/2,0),0,cv::BORDER_REPLICATE);
    filter2D(pic2,pic2,-1,gaussianKernelY,cv::Point(0,(gaussianKernelY.rows+1)/2),0,cv::BORDER_REPLICATE);

    for(int i = 1; i < nScales; i++)
    {
        pic1Scales.push_back(cv::Mat::zeros(ny[i],nx[i],CV_32FC1));
        pic2Scales.push_back(cv::Mat::zeros(ny[i],nx[i],CV_32FC1));
        resize(pic1Scales[i-1],pic1Scales[i],cv::Size(nx[i],ny[i]),scaleFactor, scaleFactor, cv::INTER_CUBIC);
        resize(pic2Scales[i-1],pic2Scales[i],cv::Size(nx[i],ny[i]),scaleFactor, scaleFactor, cv::INTER_CUBIC);
    }



    for(int s = nScales-1; s>=0; s--)
    {
        //Mat image2Warped = Mat::zeros(ny[s], nx[s], CV_32FC1);
        std::vector<cv::Mat> resultCLG = CLG(pic1Scales[s],pic2Scales[s],iterations,alpha,w,rho, nu[s], nv[s]);
        nu[s] = resultCLG[0].clone();
        nv[s] = resultCLG[1].clone();

        if(s == 0) //this was the last scale
            break;

        double factorX = ((double)nx[s-1] / nx[s]);
        double factorY = ((double)ny[s-1] / ny[s]);
        resize(nu[s],nu[s-1],cv::Size(nx[s-1],ny[s-1]),factorX,factorY, cv::INTER_CUBIC);
        resize(nv[s],nv[s-1],cv::Size(nx[s-1],ny[s-1]),factorX,factorY, cv::INTER_CUBIC);

        nu[s-1] *= 1/scaleFactor;
        nv[s-1] *= 1/scaleFactor;

    }

    std::vector<cv::Mat> result;
    result.push_back(nu[0]);
    result.push_back(nv[0]);

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
