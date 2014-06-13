#include "mainwindow.h"
#include <QApplication>

#include "opticalflowhornschunck.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Adding Algorithm
    w.addAlgorithm(new OpticalFlowHornSchunck());

    w.show();

    return a.exec();
}
