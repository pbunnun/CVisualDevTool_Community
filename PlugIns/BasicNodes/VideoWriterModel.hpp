#ifndef VIDEOWRITERMODEL_HPP
#define VIDEOWRITERMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QFileDialog>
#include <QFileInfo>

#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"
#include "IntegerData.hpp"
#include "SyncData.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "VideoWriterEmbeddedWidget.hpp"

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;


class VideoWriterModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    VideoWriterModel();

    virtual
    ~VideoWriterModel() override {}

    unsigned int
    nPorts(PortType portType) const override;

    NodeDataType
    dataType( PortType portType, PortIndex portIndex ) const override;

    std::shared_ptr< NodeData >
    outData( PortIndex port ) override;

    void
    setInData( std::shared_ptr< NodeData > nodeData, PortIndex ) override;

    QWidget *
    embeddedWidget() override { return mpEmbeddedWidget; }

    QPixmap
    minPixmap() const override { return _minPixmap; }

    static const QString _category;

    static const QString _model_name;

private Q_SLOTS :

    void em_button_clicked(int button);

private:

    VideoWriterEmbeddedWidget* mpEmbeddedWidget;

    std::shared_ptr< CVImageData > mpCVImageInData {nullptr};
    std::shared_ptr< IntegerData > mpIntegerInData {nullptr};
    std::shared_ptr< InformationData > mpInformationData {nullptr};
    std::shared_ptr<SyncData> mpSyncData {nullptr};

    QFileDialog mQFSaveDialog;

    QPixmap _minPixmap;

    bool mbPaused = false;
    size_t mzCurrentFrames = 0;
    QString mQSFilePath = "";
    std::vector<cv::Mat> mvCVImageBuffer = {};

    const unsigned int muiReserveRate = 5;

    void InsertToBuffer(const std::shared_ptr<CVImageData>& in);
    void WriteVideo(const std::shared_ptr<IntegerData>& in,
                    std::shared_ptr<InformationData> &out);

};

#endif // VIDEOWRITERMODEL_HPP
