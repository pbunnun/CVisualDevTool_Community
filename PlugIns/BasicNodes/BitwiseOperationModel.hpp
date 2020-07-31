#ifndef BITWISEOPERATIONMODEL_HPP
#define BITWISEOPERATIONMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "BitwiseOperationEmbeddedWidget.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct BitwiseOperationParameters{
    int miBitwiseType;
    BitwiseOperationParameters()
        : miBitwiseType(0)
    {
    }
} BitwiseOperationParameters;

typedef struct BitwiseOperationProperties //Possible to be made deprecated
{
    bool mbActiveMask;
    BitwiseOperationProperties()
        : mbActiveMask(false)
    {
    }
} BitwiseOperationProperties;

class BitwiseOperationModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    BitwiseOperationModel();

    virtual
    ~BitwiseOperationModel() override {}

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
    setInData(std::shared_ptr<NodeData> nodeData, PortIndex) override;

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private:
    BitwiseOperationParameters mParams;
    BitwiseOperationProperties mProps;
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<CVImageData> mapCVImageInData[3] { {nullptr} };
    BitwiseOperationEmbeddedWidget* mpEmbeddedWidget {nullptr};
    QPixmap _minPixmap;

    void processData(const std::shared_ptr<CVImageData> (&in)[3], std::shared_ptr<CVImageData>& out, const BitwiseOperationParameters& params, BitwiseOperationProperties &props);

};

#endif // BITWISEOPERATIONMODEL_HPP
