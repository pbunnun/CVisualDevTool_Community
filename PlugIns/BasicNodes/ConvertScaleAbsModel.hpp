#ifndef CONVERTSCALEABSMODEL_HPP
#define CONVERTSCALEABSMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct ConvertScaleAbsParameters{
    double mdAlpha;
    double mdBeta;
    ConvertScaleAbsParameters()
        : mdAlpha(1),
          mdBeta(0)
    {
    }
} ConvertScaleAbsParameters;

class ConvertScaleAbsModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    ConvertScaleAbsModel();

    virtual
    ~ConvertScaleAbsModel() override {}

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
    embeddedWidget() override { return nullptr; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private:
    ConvertScaleAbsParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    QPixmap _minPixmap;

    void processData( const std::shared_ptr< CVImageData> & in, std::shared_ptr< CVImageData > & out,
                      const ConvertScaleAbsParameters & params );
};

#endif // CONVERTSCALEABSMODEL_HPP
