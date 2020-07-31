#ifndef TEST_SHARPENMODEL_H
#define TEST_SHARPENMODEL_H

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;

//A non-practical dummy class. May be edited for practice.
class Test_SharpenModel : public PBNodeDataModel
{
    Q_OBJECT

    public :

        Test_SharpenModel();
        virtual ~Test_SharpenModel() override;
        unsigned int nPorts(PortType PortType) const override;
        NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
        std::shared_ptr<NodeData> outData(PortIndex port) override;
        void setInData(std::shared_ptr<NodeData>nodeData,PortIndex) override;
        QWidget *embeddedWidget() override {return nullptr;}
        QPixmap minPixmap() const override {return _minPixmap;}

        static const QString _category;
        static const QString _model_name;

    private :

        std::shared_ptr<CVImageData> mpCVImageData = nullptr;
        QPixmap _minPixmap;

};

#endif // TEST_SHARPENMODEL_H
