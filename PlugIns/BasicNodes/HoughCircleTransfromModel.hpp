#ifndef HOUGHCIRCLETRANSFROMMODEL_HPP
#define HOUGHCIRCLETRANSFROMMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "IntegerData.hpp"
#include "StdVectorData.hpp"
#include <opencv2/imgproc.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.

typedef struct HoughCircleTransformParameters{
    int miHoughMethod;
    double mdInverseRatio;
    double mdCenterDistance;
    double mdThresholdU;
    double mdThresholdL;
    int miRadiusMin;
    int miRadiusMax;

    bool mbEnableGradient;
    HoughCircleTransformParameters()
        : miHoughMethod(cv::HOUGH_GRADIENT),
          mdInverseRatio(1),
          mdCenterDistance(10),
          mdThresholdU(200),
          mdThresholdL(100),
          miRadiusMin(5),
          miRadiusMax(20)
    {
    }
} HoughCircleTransformParameters;

class HoughCircleTransformModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    HoughCircleTransformModel();

    virtual
    ~HoughCircleTransformModel() override {}

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
    HoughCircleTransformParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    std::shared_ptr<StdVectorData<cv::Vec3f>> mpStdVectorData_CVVec3f { nullptr };
    std::shared_ptr<IntegerData> mpIntegerData { nullptr };
    QPixmap _minPixmap;

    static const std::string color[3];

    void processData( const std::shared_ptr< CVImageData> & in, std::shared_ptr< StdVectorData<cv::Vec3f> > & outVec,
                      std::shared_ptr<IntegerData> &outInt, const HoughCircleTransformParameters & params);
};

#endif // HOUGHCIRCLETRANSFROMMODEL_HPP
