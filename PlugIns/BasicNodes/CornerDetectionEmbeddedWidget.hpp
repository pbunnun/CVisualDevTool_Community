#ifndef CORNERDETECTIONEMBEDDEDWIDGET_HPP
#define CORNERDETECTIONEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class CornerDetectionEmbeddedWidget;
}

class CornerDetectionEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CornerDetectionEmbeddedWidget(QWidget *parent = nullptr);
    ~CornerDetectionEmbeddedWidget();

    bool get_maskStatus_label() const;
    void set_maskStatus_label(const bool active);

private:
    Ui::CornerDetectionEmbeddedWidget *ui;
};

#endif // CORNERDETECTIONEMBEDDEDWIDGET_HPP
