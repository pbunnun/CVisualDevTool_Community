#ifndef FINDCONTOURMODEL_HPP
#define FINDCONTOURMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <opencv2/imgproc.hpp>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "StdVectorData.hpp"
#include "SyncData.hpp"


using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

typedef struct FindContourParameters{
    int miContourMode;
    int miContourMethod;
    FindContourParameters()
        : miContourMode(1),
          miContourMethod(1)
    {
    }
} FindContourParameters;

class FindContourModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    FindContourModel();

    virtual
    ~FindContourModel() override {}

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
    FindContourParameters mParams;
    std::shared_ptr<CVImageData> mpCVImageInData { nullptr };
    std::shared_ptr<StdVectorData<std::vector<cv::Point>>> mpStdVectorData_StdVector_CVPoint { nullptr };
    std::shared_ptr<StdVectorData<cv::Vec4i>> mpStdVectorData_CVVec4i {nullptr};
    std::shared_ptr<SyncData> mpSyncData {nullptr};
    QPixmap _minPixmap;

    void processData(const std::shared_ptr<CVImageData>& in, std::shared_ptr<StdVectorData<std::vector<cv::Point>>> &outVecVec,
                     std::shared_ptr<StdVectorData<cv::Vec4i>> &outVec, const FindContourParameters& params);
};

#endif // FINDCONTOURMODEL_HPP
