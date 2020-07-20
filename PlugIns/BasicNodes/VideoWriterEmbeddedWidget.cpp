#include "VideoWriterEmbeddedWidget.hpp"
#include "ui_VideoWriterEmbeddedWidget.h"

VideoWriterEmbeddedWidget::VideoWriterEmbeddedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoWriterEmbeddedWidget)
{
    ui->setupUi(this);
    ui->mpStartButton->setEnabled(true);
    ui->mpPauseButton->setEnabled(false);
    ui->mpStopButton->setEnabled(false);
    ui->mpSaveButton->setEnabled(false);
    ui->mpDisplayButton->setEnabled(false);
}

VideoWriterEmbeddedWidget::~VideoWriterEmbeddedWidget()
{
    delete ui;
}

void VideoWriterEmbeddedWidget::on_mpStartButton_clicked()
{
    Q_EMIT button_clicked_signal(0);
}

void VideoWriterEmbeddedWidget::on_mpPauseButton_clicked()
{
    Q_EMIT button_clicked_signal(1);
}

void VideoWriterEmbeddedWidget::on_mpStopButton_clicked()
{
    Q_EMIT button_clicked_signal(2);
}

void VideoWriterEmbeddedWidget::on_mpSaveButton_clicked()
{
    Q_EMIT button_clicked_signal(3);
}

void VideoWriterEmbeddedWidget::on_mpDisplayButton_clicked()
{
    Q_EMIT button_clicked_signal(4);
}

void VideoWriterEmbeddedWidget::enable_start_button(const bool enable)
{
    ui->mpStartButton->setEnabled(enable);
}

void VideoWriterEmbeddedWidget::enable_pause_button(const bool enable)
{
    ui->mpPauseButton->setEnabled(enable);
}

void VideoWriterEmbeddedWidget::enable_stop_button(const bool enable)
{
    ui->mpStopButton->setEnabled(enable);
}

void VideoWriterEmbeddedWidget::enable_save_button(const bool enable)
{
    ui->mpSaveButton->setEnabled(enable);
}

void VideoWriterEmbeddedWidget::enable_display_button(const bool enable)
{
    ui->mpDisplayButton->setEnabled(enable);
}

size_t VideoWriterEmbeddedWidget::get_frames_label() const
{
    return ui->mpFramesLabel->text().toLongLong();
}

void VideoWriterEmbeddedWidget::set_frames_label(const size_t frames)
{
    ui->mpFramesLabel->setText(QString::number(frames));
}
