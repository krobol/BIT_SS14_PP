#include "sequencewidget.h"
#include "playercontrols.h"

#include <QtWidgets>

SequenceWidget::SequenceWidget(QWidget *parent) :
    QWidget(parent)
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    QPushButton *openButton = new QPushButton(tr("Open"), this);
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    PlayerControls *controls = new PlayerControls(this);
    connect(controls, SIGNAL(play()),       this,   SLOT(playClicked()));
    connect(controls, SIGNAL(pause()),      this,   SLOT(pauseClicked()));
    connect(controls, SIGNAL(next()),       this,   SLOT(nextClicked()));
    connect(controls, SIGNAL(previous()),   this,   SLOT(previousClicked()));

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 75);

    labelDuration = new QLabel(this);
    labelDuration->setText(QString("15"));
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

}

void SequenceWidget::pauseClicked()
{

}

void SequenceWidget::nextClicked()
{

}

void SequenceWidget::previousClicked()
{

}

void SequenceWidget::seek(int seconds)
{

}

void SequenceWidget::LoadSequence(QString fileName)
{
    QImage image(fileName);
    if (image.isNull()) {
        QMessageBox::information(this, tr("Optical Flow Viewer"), tr("Cannot load %1.").arg(fileName));
        return;
    }

    currentImage = image;
    adjustImage();
}

void SequenceWidget::resizeEvent ( QResizeEvent * event )
{
    adjustImage();
}

void SequenceWidget::adjustImage()
{
    if(!currentImage.isNull())
    {
        QImage resultImage = QImage(imageLabel->size(), currentImage.format());

        QPainter painter(&resultImage);

        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(QRectF(QPointF(0,0), imageLabel->size()), Qt::black);

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        QSize center = imageLabel->size()*0.5f;

        QImage image = currentImage.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
        painter.drawImage(QRectF(QPointF(center.width() - image.width() * 0.5f, center.height() - image.height() * 0.5f), image.size()), image);

        painter.end();

        imageLabel->setPixmap(QPixmap::fromImage(resultImage));
    }
}
