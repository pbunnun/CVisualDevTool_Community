#ifndef CVIMAGEDISPLAYMODEL_HPP
#define CVIMAGEDISPLAYMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "PBImageDisplayWidget.hpp"
#include <opencv2/core.hpp>
#include "CVImageData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


typedef struct CVImageDisplayProperties
{
    std::string msImageName;
    int miChannels;
    cv::Size mCVMSizeImage;

    bool mbIsBinary;
    bool mbIsBAndW;
    bool mbIsContinuous;
    std::string msDescription;
    CVImageDisplayProperties()
        : msImageName("ImageName"),
          miChannels(0),
          mCVMSizeImage(cv::Size(0,0)),
          mbIsBinary(true),
          mbIsBAndW(true),
          mbIsContinuous(true),
          msDescription("-")
    {
    }
} CVImageDisplayProperties;


class CVImageDisplayModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    CVImageDisplayModel();

    virtual
    ~CVImageDisplayModel() override {}

    QJsonObject
    save() const override;

    void
    restore(const QJsonObject &p) override;

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType(PortType portType, PortIndex portIndex) const override;

    void
    setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    QPixmap
    minPixmap() const override { return _minPixmap; }

    bool
    resizable() const override { return true; }

    bool
    eventFilter(QObject *object, QEvent *event) override;

    static const QString _category;

    static const QString _model_name;

private:

    void display_image( );

    void processData(const std::shared_ptr< CVImageData > & in, CVImageDisplayProperties & props );

    CVImageDisplayProperties mProps;

    PBImageDisplayWidget * mpEmbeddedWidget;

    std::shared_ptr<NodeData> mpNodeData;

    std::shared_ptr<CVImageData> mpCVImageInData {nullptr};

    QPixmap _minPixmap;
};

#endif
