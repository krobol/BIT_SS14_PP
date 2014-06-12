#include "mainwindow.h"
#include <QApplication>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"

int main(int argc, char *argv[])
{
    cv::Mat mat;
    mat = cv::imread("img.JPG");
    cvNamedWindow("hello");
    cv::imshow("hello",mat);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
