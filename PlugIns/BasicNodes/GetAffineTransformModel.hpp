#ifndef GETAFFINETRANSFORMMODEL_HPP
#define GETAFFINETRANSFORMMODEL_HPP

#pragma once

#include <iostream>

#include <QDebug>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <opencv2/highgui.hpp>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"
#include "SyncData.hpp"

#include "GetAffineTransformationEmbeddedWidget.hpp"
#include "InputEventHandler.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class GetAffineTransformModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    GetAffineTransformModel();

    virtual
    ~GetAffineTransformModel() override {}

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType( PortType portType, PortIndex portIndex ) const override;

    std::shared_ptr< NodeData >
    outData( PortIndex port ) override;

    void
    setInData( std::shared_ptr< NodeData > nodeData, PortIndex ) override;

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; }

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

Q_SIGNALS :

    void onImageReady(const cv::Mat&);

private Q_SLOTS :

    void em_button_clicked();
    void onMousePressed(QMouseEvent*, cv::Mat&);
    void onMouseReleased(QMouseEvent*, cv::Mat&);
    void onMouseMoved(QMouseEvent*, cv::Mat&);

private:

    GetAffineTransformationEmbeddedWidget* mpEmbeddedWidget;
    std::shared_ptr< CVImageData > mpCVImageInData;
    std::shared_ptr< CVImageData > mpCVImageData;
    std::shared_ptr<SyncData> mpSyncData;

    QPixmap _minPixmap;

    static const QString mQSWindowName;

    void processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > & out );

private : //members shared among event handlers
    bool mbNewImage = false;
    unsigned int iterator = 0;
    cv::Mat canvas = cv::Mat();
    cv::Mat save[4] = {cv::Mat()};
    cv::Point2f holdingPoint = cv::Point();
    cv::Point2f warpArray[2][3] = {{},{}};
    bool cancel = false;
    bool holding = false;
    const cv::Scalar arrowColor[3] = {cv::Scalar(255,0,0),
                                      cv::Scalar(0,255,0),
                                      cv::Scalar(0,0,255)};
    const cv::Scalar regionColor[2] = {cv::Scalar(255,255,0),
                                       cv::Scalar(0,255,255)};

};

#endif // GETAFFINETRANSFORMMODEL_HPP
