#ifndef IMAGEDRAWINGDIALOG_HPP
#define IMAGEDRAWINGDIALOG_HPP

#include "PBImageDisplayWidget.hpp"
#include <QDialog>
#include <QMouseEvent>
#include <QDebug>
#include <opencv2/core.hpp>

namespace Ui {
class ImageDrawingDialog;
}


class ImageDrawingDialog : public QDialog //Currently instantiated only by InputEventHandler objects
{
    Q_OBJECT

public:
    explicit ImageDrawingDialog(QWidget *parent = nullptr,
                                const cv::Mat image = cv::Mat(),
                                const QString& title = "Dialog",
                                const bool modal = true);
    ~ImageDrawingDialog();

Q_SIGNALS :

    void mousePressSignal(QMouseEvent*, cv::Mat&);
    void mouseReleaseSignal(QMouseEvent*, cv::Mat&);
    void mouseMoveSignal(QMouseEvent*, cv::Mat&);

public Q_SLOTS :

    void updatedImage(const cv::Mat& image);

private Q_SLOTS :

    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

private:

    Ui::ImageDrawingDialog *ui;
    PBImageDisplayWidget* mpEmbeddedWidget;
    cv::Mat mCVImage;

};

#endif // IMAGEDRAWINGDIALOG_HPP
