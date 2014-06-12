#include "playercontrols.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>

PlayerControls::PlayerControls(QWidget *parent) :
    QWidget(parent)
  , playButton(0)
  , nextButton(0)
  , previousButton(0)
  , playerState(false)
{
    playButton = new QToolButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playButton, SIGNAL(clicked()), this, SIGNAL(play()));

    nextButton = new QToolButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));

    connect(nextButton, SIGNAL(clicked()), this, SIGNAL(next()));

    previousButton = new QToolButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));

    connect(previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(previousButton);
    layout->addWidget(playButton);
    layout->addWidget(nextButton);
    setLayout(layout);
}

void PlayerControls::setState(bool playing)
{
    if (playing)
    {
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        connect(playButton, SIGNAL(clicked()), this, SIGNAL(pause()));
    }
    else
    {
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        connect(playButton, SIGNAL(clicked()), this, SIGNAL(play()));
    }
    playerState = playing;
}
