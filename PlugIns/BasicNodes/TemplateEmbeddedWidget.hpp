#ifndef TEMPLATEEMBEDDEDWIDGET_H
#define TEMPLATEEMBEDDEDWIDGET_H

#include <QWidget>
#include <QSpinBox>

namespace Ui {
class TemplateEmbeddedWidget;
}

class TemplateEmbeddedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateEmbeddedWidget( QWidget *parent = nullptr );
    ~TemplateEmbeddedWidget();

    //Define getters and setters here.

    QStringList
    get_combobox_string_list();

    const QSpinBox *
    get_spinbox();

    void
    set_combobox_value( QString value );

    void
    set_display_text( QString value );

    void
    set_spinbox_value( int value );

    QString
    get_combobox_text();

    //Define signals emitted by widgets.

Q_SIGNALS:
    void
    button_clicked_signal( int button );

    //Define slots triggered by incoming signals.

public Q_SLOTS:
    void
    on_mpStartButton_clicked();

    void
    on_mpStopButton_clicked();

    void
    on_mpSpinBox_valueChanged( int );

    void
    on_mpComboBox_currentIndexChanged( int );

private:
    Ui::TemplateEmbeddedWidget *ui;
};

#endif // TEMPLATEEMBEDDEDWIDGET_H
