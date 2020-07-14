#ifndef GETSTRUCTURINGELEMENTMODEL_HPP
#define GETSTRUCTURINGELEMENTMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include <opencv2/imgproc.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.

typedef struct GetStructuringElementParameters{
    int miKernelShape;
    cv::Size mCVSizeKernel;
    cv::Point mCVPointAnchor;
    GetStructuringElementParameters()
        : miKernelShape(cv::MORPH_RECT),
          mCVSizeKernel(cv::Size(3,3)),
          mCVPointAnchor(cv::Point(0,0))
    {
    }
} GetStructuringElementParameters;


class GetStructuringElementModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    GetStructuringElementModel();

    virtual
    ~GetStructuringElementModel() override {}

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
    setInData(std::shared_ptr<NodeData>, PortIndex) override {}

    QWidget *
    embeddedWidget() override { return nullptr; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private:

    GetStructuringElementParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    QPixmap _minPixmap;

    void processData(std::shared_ptr<CVImageData>& out, const GetStructuringElementParameters& params);
};

#endif // GETSTRUCTURINGELEMENTMODEL_HPP
