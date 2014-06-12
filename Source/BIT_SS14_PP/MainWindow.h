#pragma once

#include "QtWidgets/qmainwindow.h"
#include "opencv2/highgui/highgui.hpp"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    MainWindow();

private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

private:
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;

    QAction *openAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

	cv::VideoCapture m_sequence;
	cv::Mat m_current_image;
};