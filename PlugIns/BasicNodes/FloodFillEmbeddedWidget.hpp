#ifndef FLOODFILLEMBEDDEDWIDGET_HPP
#define FLOODFILLEMBEDDEDWIDGET_HPP

#include <QWidget>
#include <QSpinBox>

namespace Ui {
class FloodFillEmbeddedWidget;
}

class FloodFillEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FloodFillEmbeddedWidget(QWidget *parent = nullptr);
    ~FloodFillEmbeddedWidget();

    QSpinBox* get_lowerB_spinbox();

    QSpinBox* get_lowerG_spinbox();

    QSpinBox* get_lowerR_spinbox();

    QSpinBox* get_lowerGray_spinbox();

    QSpinBox* get_upperB_spinbox();

    QSpinBox* get_upperG_spinbox();

    QSpinBox* get_upperR_spinbox();

    QSpinBox* get_upperGray_spinbox();

    void enable_lowerB_label(bool enable);

    void enable_lowerG_label(bool enable);

    void enable_lowerR_label(bool enable);

    void enable_lowerGray_label(bool enable);

    void enable_upperB_label(bool enable);

    void enable_upperG_label(bool enable);

    void enable_upperR_label(bool enable);

    void enable_upperGray_label(bool enable);

    void set_maskStatus_label(bool active);

    Ui::FloodFillEmbeddedWidget* get_ui()
    {
        return ui;
    }

Q_SIGNALS:

    void spinbox_clicked_signal(int spinbox);

private Q_SLOTS:

    void on_mpLowerBSpinbox_valueChanged();

    void on_mpLowerGSpinbox_valueChanged();

    void on_mpLowerRSpinbox_valueChanged();

    void on_mpLowerGraySpinbox_valueChanged();

    void on_mpUpperBSpinbox_valueChanged();

    void on_mpUpperGSpinbox_valueChanged();

    void on_mpUpperRSpinbox_valueChanged();

    void on_mpUpperGraySpinbox_valueChanged();

private:
    Ui::FloodFillEmbeddedWidget *ui;
};

#endif // FLOODFILLEMBEDDEDWIDGET_HPP
