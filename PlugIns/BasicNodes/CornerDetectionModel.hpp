#ifndef CORNERDETECTIONMODEL_HPP
#define CORNERDETECTIONMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "StdVectorData.hpp"
#include "SyncData.hpp"
#include "CornerDetectionEmbeddedWidget.hpp"

#include <opencv2/imgproc.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

//This is actually an implementation of cv::goodFeaturesToTrack() using either
//Harris corner detection (cv::cornerHarris) or Shi-Tomasi corner detection (cv::cornerMinEigenVal).
//See the second overload of cv::goodFeaturesToTrack() in opencv2/imgproc.hpp.

typedef struct CornerDetectionParameters{
    int miMaxCorners;
    double mdQualityLevel;
    double mdMinDistance;
    int miBlockSize;
    int miGradientSize;
    bool mbUseHarris;
    double mdFreeParameter;
    CornerDetectionParameters()
        : miMaxCorners(25),
          mdQualityLevel(0.01),
          mdMinDistance(10),
          miBlockSize(3),
          miGradientSize(3),
          mbUseHarris(false),
          mdFreeParameter(0.04)
    {
    }
} CornerDetectionParameters;

class CornerDetectionModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    CornerDetectionModel();

    virtual
    ~CornerDetectionModel() override {}

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
    CornerDetectionParameters mParams;
    CornerDetectionEmbeddedWidget* mpEmbeddedWidget;
    std::shared_ptr<CVImageData> mapCVImageInData[2] {{ nullptr }};
    std::shared_ptr<StdVectorData<cv::Point>> mpStdVectorData_CVPoint { nullptr };
    std::shared_ptr<SyncData> mpSyncData { nullptr };
    QPixmap _minPixmap;

    static const std::string color[3];

    void processData( const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr< StdVectorData<cv::Point> > & out,
                      const CornerDetectionParameters & params );
};

#endif // CORNERDETECTIONMODEL_HPP
