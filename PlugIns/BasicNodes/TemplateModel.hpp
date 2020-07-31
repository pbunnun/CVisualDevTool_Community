#ifndef TEMPLATEMODEL_HPP
#define TEMPLATEMODEL_HPP

//Include headers

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QSpinBox>
#include <QtCore/QThread>
#include <QtCore/QSemaphore>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"
#include "CVImageData.hpp"
#include "TemplateEmbeddedWidget.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

class TemplateModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    TemplateModel();

    virtual
    ~TemplateModel() override {}

    QJsonObject
    save() const override; //Saves information which is in the QtPropertyBrowser

    void
    restore(QJsonObject const &p) override; //Loads the information saved

    unsigned int
    nPorts(PortType portType) const override; //Indicates the number of ports when the node is instantiated.

    NodeDataType
    dataType(PortType portType, PortIndex portIndex) const override; //Define what data type each port receives or transmit.

    std::shared_ptr<NodeData>
    outData(PortIndex port) override; //Defines the object each output port sends out as NodeData

    void
    setInData(std::shared_ptr<NodeData>, PortIndex) override; //Defines the action when a new NodeData is sent to an input port

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; } //Defines the widget to be embedded inside the node when instantiated.

    /*
     * Recieve signals back from QtPropertyBrowser and use this function to
     * set parameters/variables accordingly.
     */
    void
    setModelProperty( QString &, const QVariant & ) override;

    /*
     * This function will be called automatically after this model is created.
     */
    void
    late_constructor() override;

    /*
     * These two static members must be defined for every models. _category can be duplicate with existing categories.
     * However, _model_name has to be a unique name.
     */
    static const QString _category;

    static const QString _model_name;

private Q_SLOTS:
    
    void
    em_button_clicked( int button ); //A slot for when the button in the embedded widget is clicked.

    void
    enable_changed(bool) override; //A slot for when the enabled checkbox is checked or unchecked.

private:
    TemplateEmbeddedWidget * mpEmbeddedWidget; //A pointer holding the widget to be embedded into the node.

    std::shared_ptr<CVImageData> mpCVImageData; //A pointer to be sent out via outData.

    bool mbCheckBox{ true };
    QString msDisplayText{ "ComboBox" };
    QSize mSize { QSize( 1, 1 ) };
    QPoint mPoint { QPoint( 7, 7 ) };
};

#endif
