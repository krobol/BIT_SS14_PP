#include "mainwindow.h"
#include <QtWidgets\qapplication.h>

#include <iostream>

int main(int argc, char *argv[])
{
	std::cout << "Creating Windows..." << std::endl;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
	
	std::cout << "Starting Program..." << std::endl;
    return a.exec();
}
