#ifndef IMAGEROIEMBEDDEDWIDGET_HPP
#define IMAGEROIEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class ImageROIEmbeddedWidget;
}

class ImageROIEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageROIEmbeddedWidget(QWidget *parent = nullptr);
    ~ImageROIEmbeddedWidget();

    void enable_apply_button(const bool enable);
    void enable_reset_button(const bool enable);   
    void enable_builtInSelector_button(const bool enable);

    bool get_useCrosshair_checkbox() const;
    bool get_fromCenter_checkbox() const;
    void set_useCrosshair_checkbox(const bool enable);
    void set_fromCenter_checkbox(const bool enable);

Q_SIGNALS :

    void button_clicked_signal( int button );

private Q_SLOTS :

    void on_mpApplyButton_clicked();

    void on_mpResetButton_clicked();

    void on_mpBuiltInSelectorButton_clicked();

private:
    Ui::ImageROIEmbeddedWidget *ui;
};

#endif // IMAGEROIEMBEDDEDWIDGET_HPP
