#include "ImageROIEmbeddedWidget.hpp"
#include "ui_ImageROIEmbeddedWidget.h"

ImageROIEmbeddedWidget::ImageROIEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageROIEmbeddedWidget)
{
    ui->setupUi(this);
    ui->mpApplyButton->setEnabled(false);
    ui->mpResetButton->setEnabled(false);
    ui->mpBuiltInSelectorButton->setEnabled(false);
}

ImageROIEmbeddedWidget::~ImageROIEmbeddedWidget()
{
    delete ui;
}

void ImageROIEmbeddedWidget::on_mpApplyButton_clicked()
{
    Q_EMIT button_clicked_signal(1);
}

void ImageROIEmbeddedWidget::on_mpResetButton_clicked()
{
    Q_EMIT button_clicked_signal(0);
}

void ImageROIEmbeddedWidget::on_mpBuiltInSelectorButton_clicked()
{
    Q_EMIT button_clicked_signal(2);
}

void ImageROIEmbeddedWidget::enable_apply_button(const bool enable)
{
    ui->mpApplyButton->setEnabled(enable);
}

void ImageROIEmbeddedWidget::enable_reset_button(const bool enable)
{
    ui->mpResetButton->setEnabled(enable);
}

void ImageROIEmbeddedWidget::enable_builtInSelector_button(const bool enable)
{
   ui->mpBuiltInSelectorButton->setEnabled(enable);
}

bool ImageROIEmbeddedWidget::get_useCrosshair_checkbox() const
{
    return ui->mpUseCrossHairCheckbok->checkState() == Qt::Checked;
}

bool ImageROIEmbeddedWidget::get_fromCenter_checkbox() const
{
    return ui->mpFromCenterCheckbox->checkState() == Qt::Checked;
}

void ImageROIEmbeddedWidget::set_useCrosshair_checkbox(const bool enable)
{
    ui->mpUseCrossHairCheckbok->setCheckState(enable? Qt::Checked : Qt::Unchecked);
}

void ImageROIEmbeddedWidget::set_fromCenter_checkbox(const bool enable)
{
    ui->mpFromCenterCheckbox->setCheckState(enable? Qt::Checked : Qt::Unchecked);
}
