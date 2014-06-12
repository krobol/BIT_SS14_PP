#ifndef SEQUENCEWIDGET_H
#define SEQUENCEWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
QT_END_NAMESPACE

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
    void seek(int seconds);

private:
    void resizeEvent ( QResizeEvent * event );
    void adjustImage();

    QSlider *slider;
    QLabel *labelDuration;

    QLabel *imageLabel;
    QImage currentImage;
};

#endif // SEQUENCEWIDGET_H
