#ifndef PIXMAPDISPLAYMODEL_HPP
#define PIXMAPDISPLAYMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class PixmapDisplayModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    PixmapDisplayModel();

    virtual
    ~PixmapDisplayModel() {}

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType(PortType portType, PortIndex portIndex) const override;

    void
    setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget *
    embeddedWidget() override { return mpQLabelImageDisplay; }

    bool
    resizable() const override { return true; }

    static const QString _category;

    static const QString _model_name;
protected:
    bool
    eventFilter(QObject *object, QEvent *event) override;

private:
    int miImageDisplayWidth;
    int miImageDisplayHeight;

    QLabel * mpQLabelImageDisplay;

    std::shared_ptr<NodeData> mpNodeData;
};
#endif
