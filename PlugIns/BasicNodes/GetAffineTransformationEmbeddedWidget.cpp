#include "GetAffineTransformationEmbeddedWidget.hpp"
#include "ui_GetAffineTransformationEmbeddedWidget.h"

GetAffineTransformationEmbeddedWidget::GetAffineTransformationEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GetAffineTransformationEmbeddedWidget)
{
    ui->setupUi(this);
    ui->mpRecreateButton->setEnabled(false);
}

GetAffineTransformationEmbeddedWidget::~GetAffineTransformationEmbeddedWidget()
{
    delete ui;
}

void GetAffineTransformationEmbeddedWidget::on_mpRecreateButton_clicked()
{
    Q_EMIT button_clicked_signal();
}

void GetAffineTransformationEmbeddedWidget::enable_recreate_button(const bool enable)
{
    ui->mpRecreateButton->setEnabled(enable);
}
