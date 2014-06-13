#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = 0);

signals:

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    QListWidget *contentsWidget;
};

#endif // CONFIGDIALOG_H
