#ifndef DRAWHISTOGRAMMODEL_HPP
#define DRAWHISTOGRAMMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "ArrayData.hpp"
#include "CVImageData.hpp"
#include "SyncData.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
//ucharbin(mod(range)==0),ucharrange_max,ucharrange_min,intthic,intlinetype
typedef struct DrawHistogramParameters{
    int miNormType;
    int miLineThickness;
    int miLineType;
    bool mbDrawEndpoints;
    bool mbEnableB;
    bool mbEnableG;
    bool mbEnableR;
    DrawHistogramParameters()
        : miNormType(cv::NORM_MINMAX),
          miLineThickness(2),
          miLineType(cv::LINE_8),
          mbDrawEndpoints(true),
          mbEnableB(true),
          mbEnableG(true),
          mbEnableR(true)
    {
    }
} DrawHistogramParameters;


class DrawHistogramModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    DrawHistogramModel();

    virtual
    ~DrawHistogramModel() override {}

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
    DrawHistogramParameters mParams;
    std::shared_ptr< ArrayData<cv::Mat,3> > mpArrayInData_CVImage { nullptr };
    std::shared_ptr< CVImageData > mpCVImageInData { nullptr };
    std::shared_ptr< CVImageData > mpCVImageData { nullptr };
    std::shared_ptr< SyncData > mpSyncData { nullptr };
    QPixmap _minPixmap;

    void
    processData( const std::shared_ptr< ArrayData<cv::Mat,3> > & in, std::shared_ptr< CVImageData > & out,
                 const DrawHistogramParameters & params );

    void
    processData( const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > & out,
                 const DrawHistogramParameters & params );
};

#endif // DRAWHISTOGRAMMODEL_HPP
