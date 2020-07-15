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

private Q_SLOTS :

    void em_button_clicked();

private:

    GetAffineTransformationEmbeddedWidget* mpEmbeddedWidget;
    std::shared_ptr< CVImageData > mpCVImageInData;
    std::shared_ptr< CVImageData > mpCVImageData;
    std::shared_ptr<SyncData> mpSyncData;

    QPixmap _minPixmap;

    typedef struct onMouseEventsParameters
    {
        cv::Mat mCVImage = cv::Mat();
        bool mbNewImage = true;

        onMouseEventsParameters() = default;
        onMouseEventsParameters(const cv::Mat& image)
            : mCVImage(image) {}
    } onMouseEventsParameters;

    static const std::string msWindowName;
    static void onMouseEvents(int event, int x, int y, int, void* pImage);

    void processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > & out );
};

#endif // GETAFFINETRANSFORMMODEL_HPP
