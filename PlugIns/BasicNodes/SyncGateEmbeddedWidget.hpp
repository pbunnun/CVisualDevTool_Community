#ifndef SYNCGATEEMBEDDEDWIDGET_HPP
#define SYNCGATEEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class SyncGateEmbeddedWidget;
}

class SyncGateEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SyncGateEmbeddedWidget(QWidget *parent = nullptr);
    ~SyncGateEmbeddedWidget();

    bool get_in0_Checkbox() const;
    bool get_in1_Checkbox() const;
    bool get_out0_Checkbox() const;
    bool get_out1_Checkbox() const;
    void set_in0_Checkbox(const bool state);
    void set_in1_Checkbox(const bool state);
    void set_out0_Checkbox(const bool state);
    void set_out1_Checkbox(const bool state);

Q_SIGNALS :
    void checkbox_checked_signal(int checkbox, int state);

private Q_SLOTS :

    void on_mpIn0Checkbox_stateChanged(int arg1);
    void on_mpIn1Checkbox_stateChanged(int arg1);
    void on_mpOut0Checkbox_stateChanged(int arg1);
    void on_mpOut1Checkbox_stateChanged(int arg1);

private:
    Ui::SyncGateEmbeddedWidget *ui;
};

#endif // SYNCGATEEMBEDDEDWIDGET_HPP
