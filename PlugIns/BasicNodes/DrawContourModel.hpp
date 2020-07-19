#ifndef DRAWCONTOURMODEL_HPP
#define DRAWCONTOURMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <opencv2/imgproc.hpp>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "StdVectorData.hpp"
#include "IntegerData.hpp"
#include "SyncData.hpp"


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

typedef struct DrawContourParameters{
    int miContourIndex;
    int mucBValue;
    int mucGValue;
    int mucRValue;
    int miLineThickness;
    int miLineType;
    DrawContourParameters()
        : miContourIndex(-1),
          mucBValue(0),
          mucGValue(255),
          mucRValue(0),
          miLineThickness(2),
          miLineType(0)
    {
    }
} DrawContourParameters;

class DrawContourModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    DrawContourModel();

    virtual
    ~DrawContourModel() override {}

    QJsonObject
    save() const override;

    void
    restore(QJsonObject const &p) override;

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData>
    outData(PortIndex port) override;

    void
    setInData(std::shared_ptr<NodeData>, PortIndex) override;

    QWidget *
    embeddedWidget() override {return nullptr;}

    /*
     * Recieve signals back from QtPropertyBrowser and use this function to
     * set parameters/variables accordingly.
     */
    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;


private:
    DrawContourParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    std::shared_ptr<StdVectorData<std::vector<cv::Point>>> mpStdVectorInData_StdVector_CVPoint {nullptr};
    std::shared_ptr<StdVectorData<cv::Vec4i>> mpStdVectorInData_CVVec4i {nullptr};
    std::shared_ptr<IntegerData> mpIntegerInData { nullptr };
    std::shared_ptr<CVImageData> mpCVImageData { nullptr };
    std::shared_ptr<IntegerData> mpIntegerData { nullptr };
    std::shared_ptr<SyncData> mpSyncData { nullptr };
    QPixmap _minPixmap;

    void processData(const std::shared_ptr<CVImageData> &inImage,
                     const std::shared_ptr<StdVectorData<std::vector<cv::Point>>>& inVecVec,
                     const std::shared_ptr<StdVectorData<cv::Vec4i>>& inVec,
                     std::shared_ptr<CVImageData> &outImage,
                     std::shared_ptr<IntegerData> &outInt,
                     const DrawContourParameters& params);

    void overwrite(const std::shared_ptr<StdVectorData<std::vector<cv::Point>>>& in, DrawContourParameters& params);
    void overwrite(const std::shared_ptr<IntegerData>& in, DrawContourParameters& params);
};

#endif // DRAWCONTOURMODEL_HPP
