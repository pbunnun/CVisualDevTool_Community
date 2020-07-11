#include "CornerDetectionEmbeddedWidget.hpp"
#include "ui_CornerDetectionEmbeddedWidget.h"

CornerDetectionEmbeddedWidget::CornerDetectionEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CornerDetectionEmbeddedWidget)
{
    ui->setupUi(this);
}

CornerDetectionEmbeddedWidget::~CornerDetectionEmbeddedWidget()
{
    delete ui;
}

bool CornerDetectionEmbeddedWidget::get_maskStatus_label() const
{
    return ui->mpMaskStatusLabel->text()=="Active";
}

void CornerDetectionEmbeddedWidget::set_maskStatus_label(const bool active)
{
    ui->mpMaskStatusLabel->setText(active? "Active" : "Inactive");
}
