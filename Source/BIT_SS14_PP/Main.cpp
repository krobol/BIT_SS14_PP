#include "mainwindow.h"

#include <QtWidgets\qapplication.h>

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[])
{
	std::cout << "Creating Window..." << std::endl;
	QApplication a(argc, argv);
    MainWindow w;
    w.show();

	std::cout << "Running Program..." << std::endl;
    return a.exec();
}
