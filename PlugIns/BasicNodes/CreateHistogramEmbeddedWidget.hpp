#ifndef CREATEHISTOGRAMEMBEDDEDWIDGET_HPP
#define CREATEHISTOGRAMEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class CreateHistogramEmbeddedWidget;
}

class CreateHistogramEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CreateHistogramEmbeddedWidget(QWidget *parent = nullptr);
    ~CreateHistogramEmbeddedWidget();

    int get_comboBox_index() const;
    void set_comboBox_index(const int index);

Q_SIGNALS :

    void comboBox_clicked_signal(int index);

private Q_SLOTS :

    void on_mpComboBox_currentIndexChanged(int index);

private:
    Ui::CreateHistogramEmbeddedWidget *ui;
};

#endif // CREATEHISTOGRAMEMBEDDEDWIDGET_HPP
