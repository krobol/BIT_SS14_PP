#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QtWidgets>

#include "AlgorithmConfig.h"

#include <iostream>
#include <functional>

class MySlider : public QSlider
{
    Q_OBJECT
public:
    MySlider ( QWidget * parent = 0 ) : QSlider(Qt::Horizontal, parent)
    {
    }

    void OnSliderCange(QLabel *label, std::string param, AlgorithmConfig* config)
    {
        m_label = label;
        m_param = param;
        m_config = config;
        m_precision = m_config->getConfigDescriptionMap().find(param)->second->precision;
    }

public slots:
    void changeLabel(int value)
    {
        char buffer[20];
        sprintf(buffer, "%.*f", m_precision, (float)value / (std::pow(10, m_precision)));
        m_label->setText(buffer);
    }

    void changeConfig(int value)
    {
        char buffer[20];
        sprintf(buffer, "%.*f", m_precision, (float)value / (std::pow(10, m_precision)));
        m_label->setText(buffer);

        m_config->setValue(m_param, (float)value / (std::pow(10, m_precision)));
    }

private:
    int             m_precision;
    QLabel          *m_label;
    std::string     m_param;
    AlgorithmConfig *m_config;
};

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = 0, AlgorithmConfig* config = nullptr);

private:
    QListWidget *contentsWidget;
};

#endif // CONFIGDIALOG_H
