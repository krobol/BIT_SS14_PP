#pragma once

#include <QtWidgets\qmainwindow.h>
#include <opencv2/highgui/highgui.hpp>

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

	bool LoadSequence(const char* path);

	void NextImage();

private:
	cv::VideoCapture m_sequence;
	cv::Mat current_image;
};