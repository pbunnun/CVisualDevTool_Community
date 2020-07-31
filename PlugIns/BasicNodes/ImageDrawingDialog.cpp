#include "ImageDrawingDialog.hpp"
#include "ui_ImageDrawingDialog.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <qopenglwidget.h>


ImageDrawingDialog::ImageDrawingDialog(QWidget *parent,
                                       const cv::Mat image,
                                       const QString& title,
                                       const bool modal) :
    QDialog(parent),
    ui(new Ui::ImageDrawingDialog),
    mpEmbeddedWidget(new PBImageDisplayWidget(this)),
    mCVImage(image.clone())
{
    ui->setupUi(this);
    this->setModal(modal);
    if(!modal)
    {
        this->setAttribute(Qt::WA_DeleteOnClose);
    }
    this->setWindowTitle(title);
    this->setFixedWidth(image.size().width);
    this->setFixedHeight(image.size().height);
    mpEmbeddedWidget->setFixedWidth(image.size().width);
    mpEmbeddedWidget->setFixedHeight(image.size().height);
    mpEmbeddedWidget->Display(mCVImage);
}

ImageDrawingDialog::~ImageDrawingDialog()
{
    delete mpEmbeddedWidget;
    delete ui;
}

void ImageDrawingDialog::mousePressEvent(QMouseEvent* event)
{
    Q_EMIT mousePressSignal(event,mCVImage);
}

void ImageDrawingDialog::mouseReleaseEvent(QMouseEvent* event)
{
    Q_EMIT mouseReleaseSignal(event,mCVImage);
}

void ImageDrawingDialog::mouseMoveEvent(QMouseEvent* event)
{
    Q_EMIT mouseMoveSignal(event,mCVImage);
}

void ImageDrawingDialog::updatedImage(const cv::Mat& image)
{
    mpEmbeddedWidget->Display(image);
}
