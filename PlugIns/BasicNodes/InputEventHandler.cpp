#include "InputEventHandler.hpp"

InputEventHandler::InputEventHandler(const cv::Mat& image, const QString& windowName)
    : PBNodeDataModel("",""),
      mpDialog( new ImageDrawingDialog(nullptr,image,windowName) )
{
    connect(mpDialog,&ImageDrawingDialog::mousePressSignal,this,&InputEventHandler::mousePressInput);
    connect(mpDialog,&ImageDrawingDialog::mouseReleaseSignal,this,&InputEventHandler::mouseReleaseInput);
    connect(mpDialog,&ImageDrawingDialog::mouseMoveSignal,this,&InputEventHandler::mouseMoveInput);
    connect(this,&InputEventHandler::updatedImageSignal,mpDialog,&ImageDrawingDialog::updatedImage);
}

InputEventHandler::~InputEventHandler()
{
    if(mpDialog)
        delete mpDialog;
}

void InputEventHandler::mousePressInput(QMouseEvent* event, cv::Mat& image)
{
    Q_EMIT mousePressSignal(event,image);
}

void InputEventHandler::mouseReleaseInput(QMouseEvent* event, cv::Mat& image)
{
    Q_EMIT mouseReleaseSignal(event,image);
}

void InputEventHandler::mouseMoveInput(QMouseEvent* event, cv::Mat& image)
{
    Q_EMIT mouseMoveSignal(event,image);
}

void InputEventHandler::imageUpdatedInput(const cv::Mat& image)
{
    Q_EMIT updatedImageSignal(image);
}

void InputEventHandler::activate() const
{
    mpDialog->exec();
}
