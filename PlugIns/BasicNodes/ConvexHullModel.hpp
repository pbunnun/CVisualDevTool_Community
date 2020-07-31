#ifndef CONVEXHULLMODEL_HPP
#define CONVEXHULLMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <opencv2/core.hpp>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "StdVectorData.hpp"
#include "SyncData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct ConvexHullParameters{
    bool mbClockwise;
    bool mbReturnPoints;
    ConvexHullParameters()
        : mbClockwise(false),
          mbReturnPoints(true) //This property is not supported yet.
    {
    }
} ConvexHullParameters;

class ConvexHullModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    ConvexHullModel();

    virtual
    ~ConvexHullModel() override {}

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
    ConvexHullParameters mParams;
    std::shared_ptr<StdVectorData<std::vector<cv::Point>>> mpStdVectorInData_StdVector_CVPoint { nullptr };
    std::shared_ptr<StdVectorData<std::vector<cv::Point>>> mpStdVectorData_StdVector_CVPoint { nullptr };
    std::shared_ptr<SyncData> mpSyncData { nullptr };
    QPixmap _minPixmap;

    void processData( const std::shared_ptr<StdVectorData<std::vector<cv::Point>>> & in,
                      std::shared_ptr<StdVectorData<std::vector<cv::Point>>> & out,
                      const ConvexHullParameters & params );

};

#endif // CONVEXHULLMODEL_HPP
