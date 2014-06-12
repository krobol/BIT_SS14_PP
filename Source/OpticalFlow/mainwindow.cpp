#include "mainwindow.h"
#include "sequencewidget.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    sequenceWidget = new SequenceWidget;

    createActions();
    createMenus();

    setCentralWidget(sequenceWidget);
    setWindowTitle(tr("Optical Flow Viewer"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open Sequence..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), sequenceWidget, SLOT(open()));

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
