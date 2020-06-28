#ifndef CVVDOLOADERMODEL_HPP
#define CVVDOLOADERMODEL_HPP

#pragma once

#include <iostream>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <nodes/DataModelRegistry>
#include "PBNodeDataModel.hpp"

#include "CVImageData.hpp"
#include <opencv2/videoio.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class VDOThread : public QThread
{
    Q_OBJECT
public:
    explicit VDOThread(QObject *parent = nullptr);
    ~VDOThread() override;
    void set_vdo_filename(QString filename);

    void resume()
    {
        mMutex.lock();
        mbPause = false;
        mMutex.unlock();
        mPauseCond.wakeAll();
    }

    void pause()
    {
        mMutex.lock();
        mbPause = true;
        mMutex.unlock();
    }

Q_SIGNALS:
    void image_ready(cv::Mat & image);

protected:
    void run() override;

private:
    bool mbAboart{false};
    bool mbCapturing{false};
    unsigned long miDelayTime;
    cv::Mat mcvImage;
    cv::VideoCapture mCVVideoCapture;

    QMutex mMutex;
    QWaitCondition mPauseCond;
    bool mbPause;
};

class CVVDOLoaderModel : public PBNodeDataModel
{
    Q_OBJECT

public:
    CVVDOLoaderModel();

    virtual
    ~CVVDOLoaderModel() override
    {
        delete mpVDOThread;
    }

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
    setInData(std::shared_ptr<NodeData>, PortIndex) override
    { }

    QWidget *
    embeddedWidget() override { return mpQLabelVDOFilename; }

    bool
    resizable() const override { return true; }

    void
    setModelProperty( QString &, const QVariant & ) override;

    static const QString _category;

    static const QString _model_name;

protected:
    bool
    eventFilter( QObject *object, QEvent *event ) override;

private Q_SLOTS:
    void
    received_image( cv::Mat & image );

    void
    enable_changed( bool ) override;

private:
    void
    set_vdo_filename( QString &, bool );

    QLabel * mpQLabelVDOFilename;
    QString msVDOFilename;

    VDOThread * mpVDOThread;

    std::shared_ptr<CVImageData> mpCVImageData;
};

#endif
