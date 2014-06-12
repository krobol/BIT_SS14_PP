#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QAction;
class QMenu;
class SequenceWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void resizeEvent ( QResizeEvent * event );

    void createActions();
    void createMenus();

    QAction *openAct;
    QAction *exitAct;

    QMenu *fileMenu;

    SequenceWidget *sequenceWidget;
};

#endif // MAINWINDOW_H
