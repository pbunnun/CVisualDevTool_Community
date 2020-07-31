#ifndef INPUTEVENtHANDLER_HPP
#define INPUTEVENtHANDLER_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>

#include "PBNodeDataModel.hpp"
#include "ImageDrawingDialog.hpp"

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

//May later add features to support all QInputEvents, including QMouseEvents and QKeyEvents
class InputEventHandler : public PBNodeDataModel //Instantiate this class when there are mouseEvents to be handled
{
    Q_OBJECT

public:
    InputEventHandler() = delete;
    InputEventHandler(const cv::Mat&, const QString&);
    virtual ~InputEventHandler();

    unsigned int nPorts(PortType) const override { return 0; }
    NodeDataType dataType(PortType, PortIndex) const override { return NodeDataType(); }
    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}
    QWidget* embeddedWidget() override { return nullptr; }

    void activate() const;

Q_SIGNALS :
    void mousePressSignal(QMouseEvent*, cv::Mat&);
    void mouseReleaseSignal(QMouseEvent*, cv::Mat&);
    void mouseMoveSignal(QMouseEvent*, cv::Mat&);
    void updatedImageSignal(const cv::Mat&);

public Q_SLOTS :
    void mousePressInput(QMouseEvent*, cv::Mat&);
    void mouseReleaseInput(QMouseEvent*, cv::Mat&);
    void mouseMoveInput(QMouseEvent*, cv::Mat&);
    void imageUpdatedInput(const cv::Mat&);

private :

    ImageDrawingDialog* mpDialog;

};

#endif // INPUTEVENtHANDLER_HPP
