#include "mainwindow.h"
#include <QApplication>

#include "opticalflowhornschunck.h"
#include "opticalflowclg.h"
#include "opticalflowclgpyramidal.h"
#include "opticalflowlucaskanade.h"
#include "opticalflowlucaskanadepyramidal.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Adding Algorithm
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowHornSchunck()));
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowCLG()));
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowClgPyramidal()));
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowLucasKanade()));
    w.addAlgorithm(OpticalFlowAlgorithmPtr(new OpticalFlowLucasKanadePyramidal()));

    w.show();

    return a.exec();
}
