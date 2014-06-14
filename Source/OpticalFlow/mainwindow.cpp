#include "mainwindow.h"

#include <QtWidgets>

#include "playercontrols.h"
#include "configdialog.h"


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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    current_frame_number(0),
    m_currentAlgorithm(nullptr),
    fps(1)
{
    QWidget* widget = new QWidget();

    createActions();
    createMenus();

    imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Dark);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    imageFrame = new QScrollArea(this);
    imageFrame->setBackgroundRole(QPalette::Dark);
    imageFrame->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imageFrame->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    imageFrame->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    imageFrame->setWidget(imageLabel);

    openButton = new QPushButton(tr("Open"), this);
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    optionsButton = new QPushButton(tr("Options"), this);
    optionsButton->setEnabled(false);
    connect(optionsButton, SIGNAL(clicked()), this, SLOT(options()));

    controls = new PlayerControls(this);
    controls->setState(false);
    connect(controls, SIGNAL(play()),       this,   SLOT(playClicked()));
    connect(controls, SIGNAL(pause()),      this,   SLOT(pauseClicked()));
    connect(controls, SIGNAL(next()),       this,   SLOT(nextClicked()));
    connect(controls, SIGNAL(previous()),   this,   SLOT(previousClicked()));
    connect(controls, SIGNAL(reset()),      this,   SLOT(resetClicked()));

    AlgorithmComboBox = new QComboBox();
    connect(AlgorithmComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(algorithmChanged(QString)));

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 0);

    labelDuration = new QLabel(this);
    labelDuration->setText(QString::number(0));
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

    QHBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->setMargin(0);
    displayLayout->addWidget(imageFrame);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(AlgorithmComboBox);
    controlLayout->addWidget(optionsButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout, 1);

    QHBoxLayout *hSliderLayout = new QHBoxLayout;
    hSliderLayout->addWidget(slider);
    hSliderLayout->addWidget(labelDuration);

    layout->addLayout(hSliderLayout);
    layout->addLayout(controlLayout);
    widget->setLayout(layout);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

    setCentralWidget(widget);

    setWindowTitle(tr("Optical Flow Viewer"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::addAlgorithm(OpticalFlowAlgorithmPtr algo)
{
    if(AlgorithmMap.find(algo->getName()) == AlgorithmMap.end())
    {
        AlgorithmMap.insert(std::pair<char*, OpticalFlowAlgorithmPtr>(algo->getName(), algo));
        AlgorithmComboBox->addItem(algo->getName());
    }
    else
    {
        AlgorithmMap[algo->getName()] = algo;
    }
    if(m_currentAlgorithm == nullptr)
    {
        m_currentAlgorithm = algo;
        setAlgorithm(m_currentAlgorithm->getName());
    }
}

void MainWindow::open()
{
    controls->setState(false);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

    if (!fileName.isEmpty())
    {
        LoadSequence(fileName);
    }
}

void MainWindow::options()
{
    if(configDialog != nullptr)
    {
        configDialog->show();
        configDialog->raise();
        configDialog->activateWindow();
    }
}

void MainWindow::playClicked()
{
    controls->setState(true);
}

void MainWindow::pauseClicked()
{
    controls->setState(false);
}

void MainWindow::nextClicked()
{
    nextImage();
}

void MainWindow::previousClicked()
{
    previousImage();
}

void MainWindow::resetClicked()
{
    m_sequence.set(CV_CAP_PROP_POS_FRAMES, 0);
    nextImage();
}

void MainWindow::seek(int frame)
{
    m_sequence.set(CV_CAP_PROP_POS_FRAMES, (double)frame);
    nextImage();
}

void MainWindow::tick()
{
    if(controls->playSequence())
        nextImage();
}

void MainWindow::algorithmChanged(const QString& text)
{
    setAlgorithm(text);
}

void MainWindow::setAlgorithm(const QString& text)
{
    if(!text.isEmpty())
    {
        if(AlgorithmMap.find(text.toStdString().c_str()) != AlgorithmMap.end())
        {
            m_currentAlgorithm = AlgorithmMap[text.toStdString().c_str()];
            if(m_currentAlgorithm != nullptr)
            {
                configDialog = new ConfigDialog();
                configDialog->changeAlgorithm(m_currentAlgorithm->getConfig());
                optionsButton->setEnabled(true);
            }
        }
    }
}

void MainWindow::LoadSequence(QString fileName)
{
    if (m_sequence.isOpened())
    {
        m_sequence.release();
        m_current_image = cv::Mat();
    }

    // Initializing the image sequence.
    m_sequence.open(fileName.toStdString().c_str());
    if (!m_sequence.isOpened())
    {
        QMessageBox::information(this, tr("Optical Flow Viewer"), tr("Failed to open image sequence %1.").arg(fileName));
        return;
    }

    controls->setState(false);
    slider->setValue(0);
    fps = m_sequence.get(CV_CAP_PROP_FPS);
    if(fps <= 1)
    {
        fps = 10;
    }
    setWindowTitle(tr("Optical Flow Viewer [FPS: ")+QString::number(fps)+tr("]"));

    timer->setInterval(1000.0/fps);
    timer->start();

    slider->setRange(0, (int)m_sequence.get(CV_CAP_PROP_FRAME_COUNT));
    nextImage();
    adjustImage();
}

void MainWindow::previousImage()
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

void MainWindow::nextImage()
{
    if (m_sequence.isOpened())
    {
        cv::Mat temp_next;

        current_frame_number = m_sequence.get(CV_CAP_PROP_POS_FRAMES);
        labelDuration->setText(QString::number(current_frame_number));

        // Getting current image.
        m_sequence >> temp_next;

        // Assuming that image sequence is at its end, if an empty image occurs.
        if(!temp_next.empty())
        {
            cv::Mat last_image;
            if(!m_current_image.empty())
            {
                last_image = m_current_image;
            }
            m_current_image = temp_next;

            // Draw Arrows here
            cv::Mat arrow_image;
            if(m_currentAlgorithm != nullptr && !last_image.empty() && !m_current_image.empty())
            {
                arrow_image = m_currentAlgorithm->drawArrows(last_image, m_current_image);
            }
            else
            {
                arrow_image = m_current_image;
            }

            displayImage = Mat2QImage(arrow_image);
            adjustImage();
        }
        else
        {
            controls->setState(false);
        }
    }
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    adjustImage();
}

void MainWindow::adjustImage()
{
    imageLabel->adjustSize();
    if(!displayImage.isNull())
    {
        imageLabel->setPixmap(QPixmap::fromImage(displayImage).scaled(imageFrame->size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open Sequence..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);
}
