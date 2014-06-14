#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "iopticalflowalgorithm.h"

#include "opencv/cv.h"
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
class ConfigDialog;

// Das Mainwindow enhält das Interface und verwaltet
// eine Liste der Algorithmen die für die Dartstellung
// des Optischen Flusses zur verfügung stehen.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addAlgorithm(OpticalFlowAlgorithmPtr algo);

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
    void resizeEvent(QResizeEvent *event);
    void LoadSequence(QString fileName);
    void setAlgorithm(const QString& text);

    void createActions();
    void createMenus();

    // Passt das Bild an die größe des Interface an
    void adjustImage();

    // Berechnet das nächste Bild mit Optical Flow Pfeilen
    void nextImage();
    // Berechnet das vorheriges Bild mit Optical Flow Pfeilen
    void previousImage();

    QAction *openAct;
    QAction *exitAct;

    QMenu *fileMenu;
    ConfigDialog *configDialog;

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
    QTimer      *timer;

    // OpenCV Image and Sequence
    cv::VideoCapture m_sequence;
    cv::Mat m_current_image;
    double current_frame_number;
    float fps;

    std::map<std::string, OpticalFlowAlgorithmPtr> AlgorithmMap;
    OpticalFlowAlgorithmPtr m_currentAlgorithm;
};

#endif // MAINWINDOW_H
