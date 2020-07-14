#include "DrawCollectionElementsEmbeddedWidget.hpp"
#include "ui_DrawCollectionElementsEmbeddedWidget.h"

DrawCollectionElementsEmbeddedWidget::DrawCollectionElementsEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawCollectionElementsEmbeddedWidget)
{
    ui->setupUi(this);
}

DrawCollectionElementsEmbeddedWidget::~DrawCollectionElementsEmbeddedWidget()
{
    delete ui;
}

void DrawCollectionElementsEmbeddedWidget::on_mpComboBox_currentIndexChanged(int index)
{
    Q_EMIT comboBox_changed_signal(index);
}

int DrawCollectionElementsEmbeddedWidget::get_comboBox_index() const
{
    return ui->mpComboBox->currentIndex();
}

void DrawCollectionElementsEmbeddedWidget::set_comboBox_index(const int &index)
{
    ui->mpComboBox->setCurrentIndex(index);
}
