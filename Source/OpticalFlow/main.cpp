#include "mainwindow.h"
#include <QApplication>

#include "opticalflowhornschunck.h"
#include "opticalflowclg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Adding Algorithm
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowCLG()));
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowHornSchunck()));

    w.show();

    return a.exec();
}
