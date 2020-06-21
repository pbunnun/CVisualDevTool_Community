#ifndef ERODEANDDILATEEMBEDDEDWIDGET_HPP
#define ERODEANDDILATEEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class ErodeAndDilateEmbeddedWidget;
}

class ErodeAndDilateEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ErodeAndDilateEmbeddedWidget(QWidget *parent = nullptr);
    ~ErodeAndDilateEmbeddedWidget();
    int getCurrentState();

Q_SIGNALS:
    void radioButton_clicked_signal();

private Q_SLOTS:

    void on_mpErodeRadioButton_clicked();
    void on_mpDilateRadioButton_clicked();

private:

    Ui::ErodeAndDilateEmbeddedWidget* ui;
    int currentState = 0;

};

#endif // ERODEANDDILATEEMBEDDEDWIDGET_HPP
