#ifndef CVIMAGELOADERMODEL_HPP
#define CVIMAGELOADERMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"
#include "PixmapData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class CVImageLoaderModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    CVImageLoaderModel();

    virtual
    ~CVImageLoaderModel() {}

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
    setInData(std::shared_ptr<NodeData>, int) override { }

    QWidget *
    embeddedWidget() override { return mpQLabelImageDisplay; }

    bool
    resizable() const override { return true; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    static const QString _category;

    static const QString _model_name;

protected:
    bool
    eventFilter(QObject *object, QEvent *event) override;

private Q_SLOTS:
    void
    enable_changed(bool) override;

private:
    void
    set_image_filename(QString &, bool);

    int miImageDisplayWidth;
    int miImageDisplayHeight;
    QString msImageFilename;

    QLabel * mpQLabelImageDisplay;

    QPixmap mQPixmap;

    std::shared_ptr<CVImageData> mpCVImageData;
};
#endif
