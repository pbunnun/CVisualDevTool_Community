#ifndef CANNYEDGEMODEL_H
#define CANNYEDGEMODEL_H

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "SyncData.hpp" //Sync output port should be implemented for heavily-computational nodes like this one

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

typedef struct CannyEdgeParameters{
    int miSizeKernel;
    int miThresholdU;
    int miThresholdL;
    bool mbEnableGradient;
    CannyEdgeParameters()
        : miSizeKernel(5),
          miThresholdU(60),
          miThresholdL(20),
          mbEnableGradient(false)
    {
    }
} CannyEdgeParameters;

class CannyEdgeModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    CannyEdgeModel();

    virtual
    ~CannyEdgeModel() override {}

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
    CannyEdgeParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<SyncData> mpSyncData { nullptr };
    QPixmap _minPixmap;

    void processData( const std::shared_ptr< CVImageData> & in, std::shared_ptr< CVImageData > & out,
                      const CannyEdgeParameters & params );
};

#endif // CANNYEDGEMODEL_H
