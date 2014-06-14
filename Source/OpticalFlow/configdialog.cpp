#include "configdialog.h"

#include <QtWidgets>

ConfigDialog::ConfigDialog(QWidget *parent, AlgorithmConfig* config) :
    QDialog(parent)
{
    if(config != nullptr)
    {
        QGroupBox *configGroup = new QGroupBox(tr("Algorithm configuration"));
        QVBoxLayout *configLayout = new QVBoxLayout;

        for(auto it = config->getConfigDescriptionMap().begin(); it != config->getConfigDescriptionMap().end(); it++)
        {
            QLabel *iterationsLabel = new QLabel(tr((*it).second->name.c_str()));
            MySlider *iterationsSlider = new MySlider(this);
            QLabel *iterationsCountLabel = new QLabel(tr(QString::number((*it).second->min).toStdString().c_str()));

            iterationsSlider->setRange((*it).second->min * (std::pow(10, (*it).second->precision)), (*it).second->max*(std::pow(10, (*it).second->precision)));
            iterationsSlider->OnSliderCange(iterationsCountLabel, (*it).first, config);

            connect(iterationsSlider, SIGNAL(sliderMoved(int)), iterationsSlider, SLOT(changeLabel(int)));
            connect(iterationsSlider, SIGNAL(valueChanged(int)), iterationsSlider, SLOT(changeConfig(int)));

            iterationsSlider->setSliderDown(true);
            iterationsSlider->setTracking(true);
            iterationsSlider->setSliderPosition(config->getValue((*it).first) * (std::pow(10, (*it).second->precision)));

            QHBoxLayout *iterationsLayout = new QHBoxLayout;
            iterationsLayout->addWidget(iterationsLabel);
            iterationsLayout->addWidget(iterationsSlider);
            iterationsLayout->addWidget(iterationsCountLabel);

            configLayout->addLayout(iterationsLayout);
        }

        configGroup->setLayout(configLayout);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(configGroup);
        mainLayout->addStretch(1);
        setLayout(mainLayout);
    }

    setWindowTitle(tr("Config Dialog"));
}
