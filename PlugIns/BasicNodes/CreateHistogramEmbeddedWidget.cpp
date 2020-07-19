#include "CreateHistogramEmbeddedWidget.hpp"
#include "ui_CreateHistogramEmbeddedWidget.h"

CreateHistogramEmbeddedWidget::CreateHistogramEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateHistogramEmbeddedWidget)
{
    ui->setupUi(this);
}

CreateHistogramEmbeddedWidget::~CreateHistogramEmbeddedWidget()
{
    delete ui;
}

void CreateHistogramEmbeddedWidget::on_mpComboBox_currentIndexChanged(int index)
{
    Q_EMIT comboBox_clicked_signal(index);
}

int CreateHistogramEmbeddedWidget::get_comboBox_index() const
{
    return ui->mpComboBox->currentIndex();
}

void CreateHistogramEmbeddedWidget::set_comboBox_index(const int index)
{
    ui->mpComboBox->setCurrentIndex(index);
}
