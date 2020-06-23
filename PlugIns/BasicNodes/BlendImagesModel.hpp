#ifndef BLENDIMAGESMODEL_HPP
#define BLENDIMAGESMODEL_HPP

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

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.

typedef struct BlendImagesParameters{
    double mdAlpha;
    double mdBeta;
    double mdGamma;
    bool mbSizeFromPort0;
    BlendImagesParameters()
        : mdAlpha(0.5),
          mdBeta(0.5),
          mdGamma(0),
          mbSizeFromPort0(false)
    {
    }
} BlendImagesParameters;

typedef struct BlendImagesProperties
{
    std::shared_ptr<CVImageData> mapImageRecord[2];
    BlendImagesProperties()
        : mapImageRecord{nullptr}
    {
    }
} BlendImagesProperties;

class BlendImagesModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    BlendImagesModel();

    virtual
    ~BlendImagesModel() override {}

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
    setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex) override;

    QWidget *
    embeddedWidget() override { return nullptr; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private:
    BlendImagesParameters mParams;
    BlendImagesProperties mProps;
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<CVImageData> mapCVImageInData[2] { {nullptr} };
    QPixmap _minPixmap;

    void processData( const std::shared_ptr< CVImageData> (&in)[2], std::shared_ptr< CVImageData > & out,
                      const BlendImagesParameters & params, BlendImagesProperties &props);
    bool allports_are_active(const std::shared_ptr<CVImageData> (&ap)[2] ) const;
};

#endif // BLENDIMAGESMODEL_HPP
