#ifndef WARPAFFINEMODEL_HPP
#define WARPAFFINEMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <opencv2/imgproc.hpp>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "CVSizeData.hpp"
#include "SyncData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct WarpAffineParameters{
    cv::Size mCVSizeOutput;
    int miFlags;
    int miBorderType;
    int mucBorderColor[3];
    WarpAffineParameters()
        : mCVSizeOutput(cv::Size(300,300)),
          miFlags(cv::INTER_LINEAR),
          miBorderType(cv::BORDER_CONSTANT),
          mucBorderColor{0}
    {
    }
} WarpAffineParameters;

class WarpAffineModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    WarpAffineModel();

    virtual
    ~WarpAffineModel() override {}

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
    WarpAffineParameters mParams;
    std::shared_ptr<CVImageData> mapCVImageInData[2] { {nullptr} };
    std::shared_ptr<CVSizeData> mpCVSizeInData;
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<SyncData> mpSyncData { nullptr };
    QPixmap _minPixmap;

    static const std::string color[3];

    void processData( const std::shared_ptr< CVImageData> (&in)[2], std::shared_ptr< CVImageData > & out,
                      const WarpAffineParameters & params );

    void overwrite( const std::shared_ptr<CVSizeData>& in, WarpAffineParameters& params);
};

#endif // WARPAFFINEMODEL_HPP
