#ifndef IMAGEROIMODEL_HPP
#define IMAGEROIMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "PBImageDisplayWidget.hpp"
#include <opencv2/highgui.hpp>
#include "CVImageData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct ImageROIParameters
{
    cv::Point mCVPointRect1;
    cv::Point mCVPointRect2;
    int mucLineColor[4];
    int miLineThickness;
    ImageROIParameters()
        : mCVPointRect1(cv::Point(0,0)),
          mCVPointRect2(cv::Point(0,0)),
          mucLineColor{0},
          miLineThickness(2)
    {
    }
} ImageROIParameters;


class ImageROIModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    ImageROIModel();

    virtual
    ~ImageROIModel() override {}

    QJsonObject
    save() const override;

    void
    restore(const QJsonObject &p) override;

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData>
    outData(PortIndex port) override;

    void
    setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    bool
    resizable() const override { return true; }

    bool
    eventFilter(QObject *object, QEvent *event) override;

    static const QString _category;

    static const QString _model_name;

private:

    void display_image();

    void processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> &out, const ImageROIParameters & params );

    static const std::string color[3];

    ImageROIParameters mParams;

    PBImageDisplayWidget * mpEmbeddedWidget;

    std::shared_ptr<NodeData> mpNodeData;

    std::shared_ptr<CVImageData> mpCVImageInData {nullptr};

    std::shared_ptr<CVImageData> mpCVImageData {nullptr};

    QPixmap _minPixmap;
};

#endif // IMAGEROIMODEL_HPP
