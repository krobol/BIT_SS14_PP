#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "iopticalflowalgorithm.h"
#include "opencv2/highgui/highgui.hpp"

QT_BEGIN_NAMESPACE
class QLabel;
class QScrollArea;
class QSlider;
class QAction;
class QMenu;
class SequenceWidget;
class QString;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

class PlayerControls;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addAlgorithm(IOpticalFlowAlgorithm* algo);

public slots:
    void open();
    void options();
    void playClicked();
    void pauseClicked();
    void nextClicked();
    void previousClicked();
    void resetClicked();
    void seek(int frame);
    void tick();

    void algorithmChanged(const QString& text);

private:
    void LoadSequence(QString fileName);
    void resizeEvent(QResizeEvent *event);

    void createActions();
    void createMenus();

    void adjustImage();

    void nextImage();
    void previousImage();

    QAction *openAct;
    QAction *exitAct;

    QMenu *fileMenu;

    // Slider
    QSlider *slider;
    QLabel *labelDuration;

    // Buttons
    QPushButton *openButton;
    QPushButton *optionsButton;

    // Player Controls
    PlayerControls *controls;

    // Algorithm
    QComboBox *AlgorithmComboBox;

    // Video Presentation
    QScrollArea *imageFrame;
    QLabel      *imageLabel;

    QImage      displayImage;
    QString     fileName;

    // OpenCV Image and Sequence
    cv::VideoCapture m_sequence;
    cv::Mat m_current_image;
    double current_frame_number;

    std::map<std::string, IOpticalFlowAlgorithm*> AlgorithmMap;
    IOpticalFlowAlgorithm* m_currentAlgorithm;
};

#endif // MAINWINDOW_H
