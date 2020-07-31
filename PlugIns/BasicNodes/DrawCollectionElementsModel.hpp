#ifndef DRAWCOLLECTIONELEMENTSMODEL_HPP
#define DRAWCOLLECTIONELEMENTSMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "StdVectorData.hpp"
#include "SyncData.hpp"
#include <opencv2/imgproc.hpp>

#include "DrawCollectionElementsEmbeddedWidget.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct DrawCollectionElementsParameters{
    bool mbDisplayLines;
    int mucLineColor[3];
    int miLineThickness;
    int miLineType;
    DrawCollectionElementsParameters()
        : mbDisplayLines(true),
          mucLineColor{0},
          miLineThickness(3),
          miLineType(cv::LINE_AA)
    {
    }
} DrawCollectionElementsParameters;

class DrawCollectionElementsModel : public PBNodeDataModel //Designed to support visualizing non-image outputs from various imgproc functions
{
    Q_OBJECT

public:
    DrawCollectionElementsModel();

    virtual
    ~DrawCollectionElementsModel() override {}

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

private Q_SLOTS :

    void em_comboBox_changed(int index);

private:
    DrawCollectionElementsParameters mParams;
    DrawCollectionElementsEmbeddedWidget* mpEmbeddedWidget;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };

    std::shared_ptr<StdVectorData<cv::Point>> mpStdVectorInData_CVPoint { nullptr };
    std::shared_ptr<StdVectorData<cv::Vec3f>> mpStdVectorInData_CVVec3f { nullptr };

    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<SyncData> mpSyncData {nullptr};
    QPixmap _minPixmap;

    static const std::string color[3];

    void processData( const std::shared_ptr< CVImageData> &inImage, const std::shared_ptr< StdVectorData<cv::Point>> &inVecPoint,
                      std::shared_ptr<CVImageData> &out, const DrawCollectionElementsParameters & params);

    void processData( const std::shared_ptr< CVImageData> &inImage, const std::shared_ptr< StdVectorData<cv::Vec3f>> &inVecPoint,
                      std::shared_ptr<CVImageData> &out, const DrawCollectionElementsParameters & params);
};

#endif // DRAWCOLLECTIONELEMENTSMODEL_HPP
