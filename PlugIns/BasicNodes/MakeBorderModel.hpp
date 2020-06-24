#ifndef MAKEBORDERMODEL_HPP
#define MAKEBORDERMODEL_HPP

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

typedef struct MakeBorderParameters{
    int miBorderTop;
    int miBorderBottom;
    int miBorderLeft;
    int miBorderRight;
    int miBorderType;
    int mucBorderColor[3];
    bool mbEnableGradient;
    MakeBorderParameters()
        : miBorderTop(10),
          miBorderBottom(10),
          miBorderLeft(10),
          miBorderRight(10),
          miBorderType(cv::BORDER_CONSTANT),
          mucBorderColor{255}
    {
    }
} MakeBorderParameters;

typedef struct MakeBorderProperties
{
    cv::Size mCVSizeInput;
    cv::Size mCVSizeOutput;
    MakeBorderProperties()
        : mCVSizeInput(cv::Size(0,0)),
          mCVSizeOutput(cv::Size(0,0))
    {
    }
} MakeBorderProperties;

class MakeBorderModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    MakeBorderModel();

    virtual
    ~MakeBorderModel() override {}

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
    MakeBorderParameters mParams;
    MakeBorderProperties mProps;
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    QPixmap _minPixmap;

    void processData( const std::shared_ptr< CVImageData> & in, std::shared_ptr< CVImageData > & out,
                      const MakeBorderParameters & params, MakeBorderProperties &props);
};

#endif // MAKEBORDERMODEL_HPP
