#ifndef VIDEOWRITEREMBEDDEDWIDGET_HPP
#define VIDEOWRITEREMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class VideoWriterEmbeddedWidget;
}

class VideoWriterEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWriterEmbeddedWidget(QWidget *parent = nullptr);
    ~VideoWriterEmbeddedWidget();

    void enable_start_button(const bool enable);
    void enable_pause_button(const bool enable);
    void enable_stop_button(const bool enable);
    void enable_save_button(const bool enable);
    void enable_display_button(const bool enable);
    size_t get_frames_label() const;
    void set_frames_label(const size_t frames);

Q_SIGNALS :

    void button_clicked_signal(int button);

private Q_SLOTS :

    void on_mpStartButton_clicked();

    void on_mpPauseButton_clicked();

    void on_mpStopButton_clicked();

    void on_mpSaveButton_clicked();

    void on_mpDisplayButton_clicked();

private:
    Ui::VideoWriterEmbeddedWidget *ui;
};

#endif // VIDEOWRITEREMBEDDEDWIDGET_HPP
