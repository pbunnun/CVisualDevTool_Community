﻿#include "CVVDOLoaderModel.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include "qtvariantproperty.h"

VDOThread::
VDOThread(QObject *parent)
    : QThread(parent)
{

}

VDOThread::
~VDOThread()
{
    mbAboart = true;
    mPauseCond.wakeAll();
    wait();
}

void
VDOThread::
run()
{
    while( !mbAboart )
    {
        mMutex.lock();
        if( mbPause )
            mPauseCond.wait(&mMutex);
        mMutex.unlock();
        if( mbCapturing )
        {
            mCVVideoCapture >> mcvImage;
            if( !mcvImage.empty() )
                Q_EMIT image_ready(mcvImage);
            else
                mCVVideoCapture.set(cv::CAP_PROP_POS_FRAMES, -1);
        }
        msleep(miDelayTime);
    }
}

void
VDOThread::
set_vdo_filename(QString filename)
{
    if( mbCapturing )
        mCVVideoCapture.release();
    mCVVideoCapture = cv::VideoCapture(filename.toStdString());
    if( mCVVideoCapture.isOpened() )
    {
        double fps = mCVVideoCapture.get(cv::CAP_PROP_FPS);
        if( fps != 0. )
            miDelayTime = unsigned(1000./fps);
        else
            miDelayTime = 10;
    }
    if( !mbCapturing )
    {
        mbCapturing = true;
        start();
    }
}

CVVDOLoaderModel::
CVVDOLoaderModel()
    : PBNodeDataModel( _model_name ),
      mpQLabelVDOFilename( new QLabel( "Click Me" )),
      mpVDOThread( new VDOThread( this ) )
{
    mpQLabelVDOFilename->setAlignment( Qt::AlignVCenter | Qt::AlignHCenter );

    mpQLabelVDOFilename->setFixedSize( 70, 20 );

    mpQLabelVDOFilename->installEventFilter( this );

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpVDOThread, &VDOThread::image_ready, this, &CVVDOLoaderModel::received_image );

    mpCVImageData = std::make_shared<CVImageData>(cv::Mat());

    FilePathPropertyType filePathPropertyType;
    filePathPropertyType.msFilename = msVDOFilename;
    filePathPropertyType.msFilter = "*.mp4;*.mpg;*.wmv;*.avi";
    filePathPropertyType.msMode = "open";
    QString propId = "filename";
    auto propFileName = std::make_shared<TypedProperty<FilePathPropertyType>>("Filename", propId, QtVariantPropertyManager::filePathTypeId(), filePathPropertyType);
    mvProperty.push_back(propFileName);
    mMapIdToProperty[propId] = propFileName;
}

void
CVVDOLoaderModel::
received_image(cv::Mat & image)
{
    mpCVImageData->set_image( image );

    Q_EMIT dataUpdated( 0 );
}

unsigned int
CVVDOLoaderModel::
nPorts(PortType portType) const
{
    switch( portType )
    {
    case PortType::In:
        return( 0 );
    case PortType::Out:
        return( 1 );
    default:
        return( 0 );
    }
}

bool
CVVDOLoaderModel::
eventFilter(QObject *object, QEvent *event)
{
    if (object == mpQLabelVDOFilename)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {

            QString filename =
                    QFileDialog::getOpenFileName(nullptr,
                                                 tr("Open Video"),
                                                 QDir::homePath(),
                                                 tr("Video Files (*.avi *.mp4)"));
            if( !filename.isEmpty() )
            {
                set_vdo_filename(filename, true);
                return true;
            }
        }
    }

    return false;
}

NodeDataType
CVVDOLoaderModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if( portType == PortType::Out && portIndex == 0 )
        return CVImageData().type();
    else
        return NodeDataType();
}

std::shared_ptr<NodeData>
CVVDOLoaderModel::
outData(PortIndex portIndex)
{
    std::shared_ptr<NodeData> result;
    if( isEnable() && portIndex == 0 && mpCVImageData->image().data != nullptr )
        result = mpCVImageData;
    return result;
}

QJsonObject
CVVDOLoaderModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();
    if( !msVDOFilename.isEmpty() )
    {
        QJsonObject cParams;
        cParams["filename"] = msVDOFilename;
        modelJson["cParams"] = cParams;
    }
    return modelJson;
}

void
CVVDOLoaderModel::
restore(const QJsonObject &p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p["cParams"].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj["filename"];
        if( !v.isUndefined() )
        {
            QString filename = v.toString();
            set_vdo_filename(filename, false);
        }
    }
}

void
CVVDOLoaderModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    if( id == "filename" )
    {
        QString filename = value.toString();
        set_vdo_filename( filename, false );
    }
}

void
CVVDOLoaderModel::
set_vdo_filename(QString & filename, bool bEmitSignal )
{
    auto prop = mMapIdToProperty["filename"];
    auto typedProp = std::static_pointer_cast<TypedProperty<FilePathPropertyType>>( prop );
    typedProp->getData().msFilename = filename;
    if( bEmitSignal )
        Q_EMIT property_changed_signal( prop );

    if( msVDOFilename == filename )
        return;

    msVDOFilename = filename;

    QFileInfo file(filename);
    mpQLabelVDOFilename->setText(file.fileName());

    mpVDOThread->set_vdo_filename(msVDOFilename);
    if( isEnable() )
        mpVDOThread->resume();
    else
        mpVDOThread->pause();
}

void
CVVDOLoaderModel::
enable_changed(bool enable)
{
    if( enable )
        mpVDOThread->resume();
    else
        mpVDOThread->pause();
}

const QString CVVDOLoaderModel::_category = QString( "Source" );

const QString CVVDOLoaderModel::_model_name = QString( "CV VDO Loader" );
