#include "FloodFillEmbeddedWidget.hpp"
#include "ui_FloodFillEmbeddedWidget.h"

FloodFillEmbeddedWidget::FloodFillEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FloodFillEmbeddedWidget)
{
    ui->setupUi(this);
    ui->mpLowerBSpinbox->setRange(0,255);
    ui->mpLowerGSpinbox->setRange(0,255);
    ui->mpLowerRSpinbox->setRange(0,255);
    ui->mpLowerGraySpinbox->setRange(0,255);
    ui->mpUpperBSpinbox->setRange(0,255);
    ui->mpUpperGSpinbox->setRange(0,255);
    ui->mpUpperRSpinbox->setRange(0,255);
    ui->mpUpperGraySpinbox->setRange(0,255);
}

FloodFillEmbeddedWidget::~FloodFillEmbeddedWidget()
{
    delete ui;
}

void FloodFillEmbeddedWidget::on_mpLowerBSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(0);
}

void FloodFillEmbeddedWidget::on_mpLowerGSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(1);
}

void FloodFillEmbeddedWidget::on_mpLowerRSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(2);
}

void FloodFillEmbeddedWidget::on_mpLowerGraySpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(3);
}

void FloodFillEmbeddedWidget::on_mpUpperBSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(4);
}

void FloodFillEmbeddedWidget::on_mpUpperGSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(5);
}

void FloodFillEmbeddedWidget::on_mpUpperRSpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(6);
}

void FloodFillEmbeddedWidget::on_mpUpperGraySpinbox_valueChanged()
{
    Q_EMIT spinbox_clicked_signal(7);
}

QSpinBox* FloodFillEmbeddedWidget::get_lowerB_spinbox()
{
    return ui->mpLowerBSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_lowerG_spinbox()
{
    return ui->mpLowerGSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_lowerR_spinbox()
{
    return ui->mpLowerRSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_lowerGray_spinbox()
{
    return ui->mpLowerGraySpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_upperB_spinbox()
{
    return ui->mpUpperBSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_upperG_spinbox()
{
    return ui->mpUpperGSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_upperR_spinbox()
{
    return ui->mpUpperRSpinbox;
}

QSpinBox* FloodFillEmbeddedWidget::get_upperGray_spinbox()
{
    return ui->mpUpperGraySpinbox;
}

void FloodFillEmbeddedWidget::enable_lowerB_label(bool enable)
{
    ui->mpLowerBLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_lowerG_label(bool enable)
{
    ui->mpLowerGLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_lowerR_label(bool enable)
{
    ui->mpLowerRLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_lowerGray_label(bool enable)
{
    ui->mpLowerGrayLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_upperB_label(bool enable)
{
    ui->mpUpperBLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_upperG_label(bool enable)
{
    ui->mpUpperGLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_upperR_label(bool enable)
{
    ui->mpUpperRLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::enable_upperGray_label(bool enable)
{
    ui->mpUpperGrayLabel->setEnabled(enable);
}

void FloodFillEmbeddedWidget::set_maskStatus_label(bool active)
{
    ui->mpMaskStatusLabel->setText(active? "Active" : "Inactive");
}
