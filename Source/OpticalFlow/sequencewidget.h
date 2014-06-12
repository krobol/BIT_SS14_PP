#ifndef SEQUENCEWIDGET_H
#define SEQUENCEWIDGET_H

#include <QWidget>

#include "opencv2/highgui/highgui.hpp"

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
QT_END_NAMESPACE

class PlayerControls;

class SequenceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SequenceWidget(QWidget *parent = 0);

    void LoadSequence(QString fileName);

signals:

public slots:
    void open();

    void playClicked();
    void pauseClicked();
    void nextClicked();
    void previousClicked();
    void seek(int frame);
    void tick();

private:
    void resizeEvent ( QResizeEvent * event );
    void adjustImage();

    void nextImage();
    void previousImage();

    PlayerControls *controls;

    QSlider *slider;
    QLabel *labelDuration;

    QLabel *imageLabel;
    QImage displayImage;
    QString fileName;

    cv::VideoCapture m_sequence;
    cv::Mat m_current_image;
    double current_frame_number;
};

#endif // SEQUENCEWIDGET_H
