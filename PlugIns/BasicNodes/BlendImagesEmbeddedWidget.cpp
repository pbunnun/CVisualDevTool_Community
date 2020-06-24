#include "BlendImagesEmbeddedWidget.hpp"
#include "ui_BlendImagesEmbeddedWidget.h"

BlendImagesEmbeddedWidget::BlendImagesEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlendImagesEmbeddedWidget)
{
    ui->setupUi(this);
    ui->mpAddWeightedRadioButton->setChecked(true);
}

BlendImagesEmbeddedWidget::~BlendImagesEmbeddedWidget()
{
    delete ui;
}

void BlendImagesEmbeddedWidget::on_mpAddRadioButton_clicked()
{
    currentState = 0;
    Q_EMIT radioButton_clicked_signal();
}

void BlendImagesEmbeddedWidget::on_mpAddWeightedRadioButton_clicked()
{
    currentState = 1;
    Q_EMIT radioButton_clicked_signal();
}

int BlendImagesEmbeddedWidget::getCurrentState()
{
    return currentState;
}
