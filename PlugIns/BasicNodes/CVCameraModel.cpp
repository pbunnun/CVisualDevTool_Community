#include "CVCameraModel.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QtWidgets/QFileDialog>
#include <QVariant>
#include "qtvariantproperty.h"

CVCameraThread::
CVCameraThread(QObject *parent)
    : QThread(parent)
{

}

CVCameraThread::
~CVCameraThread()
{
    mbAboart = true;
    mSingleShotSemaphore.release();
    wait();
}

void
CVCameraThread::
run()
{
    while(!mbAboart)
    {
        if( mCameraCheckSemaphore.tryAcquire() )
            check_camera();
        if( mbConnected )
        {
            if( mbSingleShotMode )
            {
                mSingleShotSemaphore.acquire();

                mCVVideoCapture >> mCVImage;
                if( !mCVImage.empty() )
                    Q_EMIT image_ready( mCVImage );
                else
                    mCVVideoCapture.set(cv::CAP_PROP_POS_FRAMES, -1);
            }
            else
            {
                mCVVideoCapture >> mCVImage;
                if( !mCVImage.empty() )
                    Q_EMIT image_ready( mCVImage );
                else
                    mCVVideoCapture.set(cv::CAP_PROP_POS_FRAMES, -1);
            }
        }
        msleep( miDelayTime );
   }
}

void
CVCameraThread::
set_camera_id(int camera_id)
{
    if( miCameraID != camera_id )
    {
        miCameraID = camera_id;
        mCameraCheckSemaphore.release();
        mSingleShotSemaphore.release();
        if( !isRunning() )
            start();
    }
}

// need to move this function to run in a thread, eg. run function, otherwise it will block a main GUI loop a bit.
void
CVCameraThread::
check_camera()
{
    if( mbConnected )
    {
        mCVVideoCapture.release();
        mbConnected = false;
    }
    if( miCameraID != -1 )
    {
        try
        {
            mCVVideoCapture = cv::VideoCapture(miCameraID);
            if( mCVVideoCapture.isOpened() )
            {
                double fps = mCVVideoCapture.get(cv::CAP_PROP_FPS);
                if( fps != 0. )
                    miDelayTime = unsigned(1000./fps);
                else
                    miDelayTime = 10;
                Q_EMIT camera_ready(true);
            }
            else
                Q_EMIT camera_ready(false);
            if( !mbConnected )
            {
                mbConnected = true;
            }
        }
        catch ( cv::Exception &e) {
            qDebug() << e.what();
            Q_EMIT camera_ready( false );
        }
    }
    else
        Q_EMIT camera_ready( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CVCameraModel::
CVCameraModel()
    : PBNodeDataModel( _model_name, false ),
      mpEmbeddedWidget( new CVCameraEmbeddedWidget() )
{
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &CVCameraEmbeddedWidget::button_clicked_signal, this, &CVCameraModel::em_button_clicked );

    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpInformationData = std::make_shared< InformationData >( );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList( {"0", "1", "2", "3", "4"} );
    enumPropertyType.miCurrentIndex = enumPropertyType.mslEnumNames.indexOf( QString::number( mParams.miCameraID ) );
    QString propId = "camera_id";
    auto propCameraID = std::make_shared< TypedProperty< EnumPropertyType > >("Camera ID", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propCameraID );
    mMapIdToProperty[ propId ] = propCameraID;
}

void
CVCameraModel::
received_image( cv::Mat & image )
{
    mpCVImageData->set_image( image );
    updateAllOutputPorts();
}

void
CVCameraModel::
camera_status_changed( bool status )
{
    mParams.mbCameraStatus = status;
}

unsigned int
CVCameraModel::
nPorts( PortType portType ) const
{
    switch( portType )
    {
    case PortType::In:
        return( 1 );
    case PortType::Out:
        return( 2 );
    default:
        return( 0 );
    }
}

NodeDataType
CVCameraModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return SyncData().type();
    }
    else if( portType == PortType::Out )
    {
        if( portIndex == 0 )
            return CVImageData().type();
        else if( portIndex == 1 )
            return InformationData().type();
        else
            return NodeDataType();
    }
    else
        return NodeDataType();
}

std::shared_ptr<NodeData>
CVCameraModel::
outData(PortIndex portIndex)
{
    std::shared_ptr<NodeData> result;
    if( isEnable() && mpCVImageData->image().data != nullptr )
    {
        if( portIndex == 0 )
            result = mpCVImageData;
        else if( portIndex == 1 )
        {
            QString currentTime = QTime::currentTime().toString( "hh:mm:ss.zzz" ) + " :: ";
            QString sInformation = "\n";
            cv::Mat image = mpCVImageData->image();
            if( image.channels() == 1 )
                sInformation += currentTime + "Image Type : Gray\n" + currentTime + "Image Format : CV_8UC1\n";
            else if( image.channels() == 3 )
                sInformation += currentTime + "Image Type : Color\n" + currentTime + "Image Format : CV_8UC3\n";
            sInformation += currentTime + "Width x Height : " + QString::number( image.cols ) + " x " + QString::number( image.rows );
            mpInformationData->set_information( sInformation );
            result = mpInformationData;
        }
    }
    return result;
}

void
CVCameraModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if(nodeData)
    {
        auto d = std::dynamic_pointer_cast<SyncData>(nodeData);
        if( d )
            mpCVCameraThread->fire_single_shot();
    }
}

QJsonObject
CVCameraModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams[ "camera_id" ] = mParams.miCameraID;
    modelJson[ "cParams" ] = cParams;

    return modelJson;
}

void
CVCameraModel::
restore(const QJsonObject &p)
{
    PBNodeDataModel::restore(p);
    late_constructor();

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "camera_id" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "camera_id" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType> >( prop );
            typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( v.toString() );

            mParams.miCameraID = v.toInt();
        }

        mpEmbeddedWidget->set_params( mParams );
        if( isEnable() )
            mpCVCameraThread->set_camera_id( mParams.miCameraID );
    }
}

void
CVCameraModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "camera_id" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( value.toString() );

        mParams.miCameraID = value.toInt();
        mpEmbeddedWidget->set_params( mParams );
        if( isEnable() )
            mpCVCameraThread->set_camera_id( mParams.miCameraID );
    }
}

void
CVCameraModel::
enable_changed(bool enable)
{
    mpEmbeddedWidget->set_ready_state( !enable );
    if( enable )
    {
        mParams.miCameraID = mpEmbeddedWidget->get_params().miCameraID;
        mpCVCameraThread->set_camera_id(mParams.miCameraID);
    }
    else
        mpCVCameraThread->set_camera_id(-1);
}

void
CVCameraModel::
late_constructor()
{
    mpCVCameraThread = new CVCameraThread(this);
    connect( mpCVCameraThread, &CVCameraThread::image_ready, this, &CVCameraModel::received_image );
    connect( mpCVCameraThread, &CVCameraThread::camera_ready, this, &CVCameraModel::camera_status_changed );
    connect( mpCVCameraThread, &CVCameraThread::camera_ready, mpEmbeddedWidget, &CVCameraEmbeddedWidget::camera_status_changed );
}

void
CVCameraModel::
em_button_clicked( int button )
{
    if( button == 0 ) //Start
    {
        auto prop = mMapIdToProperty[ "enable" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = true;
        Q_EMIT property_changed_signal( prop );
    }
    else //Stop
    {
        auto prop = mMapIdToProperty[ "enable" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = false;
        Q_EMIT property_changed_signal( prop );

        if( button == 2 )
        {
            prop = mMapIdToProperty[ "camera_id" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( QString::number( mpEmbeddedWidget->get_params().miCameraID ) );
            Q_EMIT property_changed_signal( prop );
        }
    }
}

const QString CVCameraModel::_category = QString( "Source" );

const QString CVCameraModel::_model_name = QString( "CV Camera" );
