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
        if(m_precision > 0)
            sprintf(buffer, "%.*f", m_precision, (float)value / (std::pow(10, m_precision)));
        else
            sprintf(buffer, "%.*f", m_precision, (float)value);
        m_label->setText(buffer);
    }

    void changeConfig(int value)
    {
        if(m_precision > 0)
            m_config->setValue(m_param, (float)value / (std::pow(10, m_precision)));
        else
            m_config->setValue(m_param, (float)value);
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
    explicit ConfigDialog(QWidget *parent = 0);

    void changeAlgorithm(AlgorithmConfig* configDescription);

private:
    QListWidget *contentsWidget;
};

#endif // CONFIGDIALOG_H
