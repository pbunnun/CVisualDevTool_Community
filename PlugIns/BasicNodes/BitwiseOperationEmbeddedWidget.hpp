#ifndef BITWISEOPERATIONEMBEDDEDWIDGET_HPP
#define BITWISEOPERATIONEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class BitwiseOperationEmbeddedWidget;
}

class BitwiseOperationEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BitwiseOperationEmbeddedWidget(QWidget *parent = nullptr);
    ~BitwiseOperationEmbeddedWidget();

private:
    Ui::BitwiseOperationEmbeddedWidget *ui;
};

#endif // BITWISEOPERATIONEMBEDDEDWIDGET_HPP
