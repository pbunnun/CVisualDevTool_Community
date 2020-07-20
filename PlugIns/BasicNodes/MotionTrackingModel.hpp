#ifndef MOTIONTRACKINGMODEL_HPP
#define MOTIONTRACKINGMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"
#include "SyncData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

class MotionTrackingModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    MotionTrackingModel();

    virtual
    ~MotionTrackingModel() override {}

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType( PortType portType, PortIndex portIndex ) const override;

    std::shared_ptr< NodeData >
    outData( PortIndex port ) override;

    void
    setInData( std::shared_ptr< NodeData > nodeData, PortIndex ) override;

    QWidget *
    embeddedWidget() override { return nullptr; }

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private:
    std::shared_ptr< CVImageData > mpCVImageData;
    std::shared_ptr< SyncData > mpSyncData;

    QPixmap _minPixmap;

    static void updateHistory(std::vector<cv::Rect> availableRect);

    static cv::Scalar getColor(int placement);

    static cv::Mat processData(const std::shared_ptr<CVImageData> &p);

};
#endif
