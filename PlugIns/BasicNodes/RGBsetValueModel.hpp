#ifndef RGBSETVALUE_H
#define RGBSETVALUE_H

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

typedef struct RGBsetValueParameters
{
     uchar mucRvalue;
     uchar mucGvalue;
     uchar mucBvalue;
     RGBsetValueParameters()
     {
         mucRvalue = 0;
         mucBvalue = 0;
         mucGvalue = 0;
     }
} RGBsetValueParameters;

class RGBsetValueModel : public PBNodeDataModel
{
    Q_OBJECT

public :
    RGBsetValueModel();

    virtual
    ~RGBsetValueModel() override {}

    QJsonObject save() const override;

    void restore(const QJsonObject &p) override;

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex) override;

    QWidget *embeddedWidget() override {return nullptr;}

    void setModelProperty(QString &, const QVariant &) override;

    QPixmap minPixmap() const override {return _minPixmap;}

    static const QString _category;

    static const QString _model_name;

private :
    RGBsetValueParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageData {nullptr};
    std::shared_ptr<CVImageData> mpCVImageInData {nullptr};
    QPixmap _minPixmap;
};


#endif // RGBSETVALUE_H
