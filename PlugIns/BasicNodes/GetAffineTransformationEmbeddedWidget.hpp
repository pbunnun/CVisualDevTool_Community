#ifndef GETAFFINETRANSFORMATIONEMBEDDEDWIDGET_HPP
#define GETAFFINETRANSFORMATIONEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class GetAffineTransformationEmbeddedWidget;
}

class GetAffineTransformationEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GetAffineTransformationEmbeddedWidget(QWidget *parent = nullptr);
    ~GetAffineTransformationEmbeddedWidget();

    void enable_recreate_button(const bool enable);

Q_SIGNALS :
    void button_clicked_signal();

private Q_SLOTS :

    void on_mpRecreateButton_clicked();

private:
    Ui::GetAffineTransformationEmbeddedWidget *ui;
};

#endif // GETAFFINETRANSFORMATIONEMBEDDEDWIDGET_HPP
