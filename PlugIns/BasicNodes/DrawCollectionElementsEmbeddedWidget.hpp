#ifndef DRAWCOLLECTIONELEMENTSEMBEDDEDWIDGET_HPP
#define DRAWCOLLECTIONELEMENTSEMBEDDEDWIDGET_HPP

#include <QWidget>

namespace Ui {
class DrawCollectionElementsEmbeddedWidget;
}

class DrawCollectionElementsEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawCollectionElementsEmbeddedWidget(QWidget *parent = nullptr);
    ~DrawCollectionElementsEmbeddedWidget();

    int get_comboBox_index() const;
    void set_comboBox_index(const int& index);

Q_SIGNALS :

    void comboBox_changed_signal(int index);

private Q_SLOTS :

    void on_mpComboBox_currentIndexChanged(int index);

private:
    Ui::DrawCollectionElementsEmbeddedWidget *ui;
};

#endif // DRAWCOLLECTIONELEMENTSEMBEDDEDWIDGET_HPP
