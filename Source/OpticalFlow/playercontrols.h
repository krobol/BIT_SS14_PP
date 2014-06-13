#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
class QComboBox;
QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerControls(QWidget *parent = 0);

    bool playSequence() { return playerState; }

public slots:
    void setState(bool playing);

signals:
    void play();
    void pause();
    void next();
    void previous();
    void reset();

private:
    bool playerState;

    QAbstractButton *playButton;
    QAbstractButton *nextButton;
    QAbstractButton *previousButton;
    QAbstractButton *resetButton;
};

#endif // PLAYERCONTROLS_H
