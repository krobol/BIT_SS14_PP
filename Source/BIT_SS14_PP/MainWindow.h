#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets\qmainwindow.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
};

#endif // MAINWINDOW_H
