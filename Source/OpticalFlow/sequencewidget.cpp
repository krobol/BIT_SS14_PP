#include "sequencewidget.h"
#include "playercontrols.h"

#include <QtWidgets>

#include "opencv/cv.h"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

cv::Mat QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy jsust in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}


SequenceWidget::SequenceWidget(QWidget *parent) :
    QWidget(parent),
    current_frame_number(0)
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    QPushButton *openButton = new QPushButton(tr("Open"), this);
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    controls = new PlayerControls(this);
    controls->setState(false);
    connect(controls, SIGNAL(play()),       this,   SLOT(playClicked()));
    connect(controls, SIGNAL(pause()),      this,   SLOT(pauseClicked()));
    connect(controls, SIGNAL(next()),       this,   SLOT(nextClicked()));
    connect(controls, SIGNAL(previous()),   this,   SLOT(previousClicked()));

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 0);

    labelDuration = new QLabel(this);
    labelDuration->setText(QString::number(0));
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

    QHBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->setMargin(0);
    displayLayout->addWidget(imageLabel);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout, 1);

    QHBoxLayout *hSliderLayout = new QHBoxLayout;
    hSliderLayout->addWidget(slider);
    hSliderLayout->addWidget(labelDuration);

    layout->addLayout(hSliderLayout);
    layout->addLayout(controlLayout);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer->start();
}

void SequenceWidget::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

    if (!fileName.isEmpty())
    {
        LoadSequence(fileName);
    }
}

void SequenceWidget::playClicked()
{
    controls->setState(true);
}

void SequenceWidget::pauseClicked()
{
    controls->setState(false);
}

void SequenceWidget::nextClicked()
{
    nextImage();
}

void SequenceWidget::previousClicked()
{
    previousImage();
}

void SequenceWidget::seek(int frame)
{
    m_sequence.set(CV_CAP_PROP_POS_FRAMES, (double)frame);
}

void SequenceWidget::tick()
{
    if(controls->playSequence())
        nextImage();
}

void SequenceWidget::LoadSequence(QString fileName)
{
    //QImage image(fileName);
    //if (image.isNull()) {
    //    QMessageBox::information(this, tr("Optical Flow Viewer"), tr("Cannot load %1.").arg(fileName));
    //    return;
    //}

    // Initializing the image sequence.
    m_sequence.open(fileName.toStdString().c_str());
    if (!m_sequence.isOpened())
    {
        QMessageBox::information(this, tr("Optical Flow Viewer"), tr("Failed to open image sequence %1.").arg(fileName));
        return;
    }

    slider->setRange(0, (int)m_sequence.get(CV_CAP_PROP_FRAME_COUNT));
    nextImage();
    adjustImage();
}

void SequenceWidget::previousImage()
{
    if (m_sequence.isOpened())
    {
        current_frame_number = m_sequence.get(CV_CAP_PROP_POS_FRAMES);
        if(current_frame_number > 1.0)
        {
            m_sequence.set(CV_CAP_PROP_POS_FRAMES, current_frame_number-2.0);
            nextImage();
        }
    }
}

void SequenceWidget::nextImage()
{
    if (m_sequence.isOpened())
    {
        cv::Mat next_image;

        current_frame_number = m_sequence.get(CV_CAP_PROP_POS_FRAMES);
        labelDuration->setText(QString::number(current_frame_number));

        // Getting current image.
        m_sequence >> next_image;

        // Assuming that image sequence is at its end, if an empty image occurs.
        if(next_image.empty())
        {
            std::cout << "End of Sequence." << std::endl;
        }
        else
        {
            cv::Mat last_image;
            if(!m_current_image.empty())
            {
                last_image = m_current_image;
            }
            m_current_image = next_image;

            //ToDo: Draw Arrows here



            displayImage = Mat2QImage(m_current_image);
            adjustImage();
        }
    }
}

void SequenceWidget::resizeEvent ( QResizeEvent * event )
{
    adjustImage();
}

void SequenceWidget::adjustImage()
{
    if(!displayImage.isNull())
    {
        QImage resultImage = QImage(imageLabel->size(), displayImage.format());

        QPainter painter(&resultImage);

        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(QRectF(QPointF(0,0), imageLabel->size()), Qt::black);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        QSize center = imageLabel->size()*0.5f;

        QImage image = displayImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
        painter.drawImage(QRectF(QPointF(center.width() - image.width() * 0.5f, center.height() - image.height() * 0.5f), image.size()), image);

        painter.end();

        imageLabel->setPixmap(QPixmap::fromImage(resultImage));
    }
}
