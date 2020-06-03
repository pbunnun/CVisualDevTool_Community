#include "CVImageLoaderModel.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include "qtvariantproperty.h"

CVImageLoaderModel::
CVImageLoaderModel()
    : PBNodeDataModel( _model_name ),
      mpQLabelImageDisplay( new QLabel( "Click Me" ) )
{
    mpQLabelImageDisplay->setAlignment( Qt::AlignVCenter | Qt::AlignHCenter );

    QFont f = mpQLabelImageDisplay->font();
    f.setBold( true );
    f.setItalic( true );

    mpQLabelImageDisplay->setFont( f );

    mpQLabelImageDisplay->setFixedSize( 200, 200 );

    mpQLabelImageDisplay->installEventFilter( this );

    mpCVImageData = std::make_shared<CVImageData>( cv::Mat() );

    miImageDisplayWidth = mpQLabelImageDisplay->width();
    miImageDisplayHeight = mpQLabelImageDisplay->height();

    FilePathPropertyType filePathPropertyType;
    filePathPropertyType.msFilename = msImageFilename;
    filePathPropertyType.msFilter = "*.jpg;*.jpeg;*.bmp;*.tiff;*.tif;*.pbm;*.png";
    filePathPropertyType.msMode = "open";
    QString propId = "filename";
    auto propFileName = std::make_shared< TypedProperty<FilePathPropertyType > >( "Filename", propId, QtVariantPropertyManager::filePathTypeId(), filePathPropertyType );
    mvProperty.push_back( propFileName );
    mMapIdToProperty[ propId ] = propFileName;
}


unsigned int
CVImageLoaderModel::
nPorts( PortType portType ) const
{
    switch ( portType )
    {
    case PortType::In:
        return( 0 );
    case PortType::Out:
        return( 1 );
    default:
        return( -1 );
    }
}


bool
CVImageLoaderModel::
eventFilter( QObject *object, QEvent *event )
{
    if ( object == mpQLabelImageDisplay )
    {
        if ( event->type() == QEvent::MouseButtonPress )
        {

            QString filename =
                    QFileDialog::getOpenFileName( nullptr,
                                                 tr( "Open Image" ),
                                                 QDir::homePath(),
                                                 tr( "Image Files (*.png *.jpg *.bmp)" ) );
            if( !filename.isEmpty() )
            {
                set_image_filename( filename, true );
                return true;
            }
        }
        else if ( event->type() == QEvent::Resize )
        {
            miImageDisplayWidth = mpQLabelImageDisplay->width();
            miImageDisplayHeight = mpQLabelImageDisplay->height();
            if ( !mQPixmap.isNull() )
                mpQLabelImageDisplay->setPixmap(mQPixmap.scaled(miImageDisplayWidth, miImageDisplayHeight, Qt::KeepAspectRatio));
        }
    }

    return false;
}


NodeDataType
CVImageLoaderModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if( portType == PortType::Out && portIndex == 0 )
        return CVImageData().type();
    else
        return NodeDataType();
}


std::shared_ptr<NodeData>
CVImageLoaderModel::
outData(PortIndex portIndex)
{
    std::shared_ptr<NodeData> result;
    if( mbEnable && portIndex == 0 && mpCVImageData->image().data != nullptr )
        result = mpCVImageData;
    return result;
}


QJsonObject
CVImageLoaderModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();
    if( !msImageFilename.isEmpty() )
    {
        QJsonObject cParams;
        cParams["filename"] = msImageFilename;
        modelJson["cParams"] = cParams;
    }
    return modelJson;
}

void
CVImageLoaderModel::
restore( QJsonObject const &p )
{
    PBNodeDataModel::restore( p );

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "filename" ];
        if( !v.isUndefined() )
        {
            QString filename = v.toString();
            set_image_filename( filename, false );
        }
    }
}

void
CVImageLoaderModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    if( id == "filename" )
    {
        QString filename = value.toString();
        set_image_filename( filename, false );
    }
}

void
CVImageLoaderModel::
set_image_filename( QString & filename, bool bEmitSignal )
{
    auto prop = mMapIdToProperty[ "filename" ];
    auto typedProp = std::static_pointer_cast< TypedProperty< FilePathPropertyType > >( prop );
    typedProp->getData().msFilename = filename;
    if( bEmitSignal )
        Q_EMIT property_changed_signal( prop );

    if( msImageFilename == filename )
        return;
    msImageFilename = filename;
    std::cout << msImageFilename.toStdString();
    cv::Mat cvImage = cv::imread( msImageFilename.toStdString() );
    if( cvImage.data != nullptr )
    {
        if( cvImage.channels() == 1 )
        {
            mpCVImageData = std::make_shared< CVImageData >( cvImage );
            mQPixmap = QPixmap::fromImage( QImage( mpCVImageData->image().data, mpCVImageData->image().cols, mpCVImageData->image().rows, mpCVImageData->image().step, QImage::Format_Grayscale8 ) );
        }
        else
        {
            cv::cvtColor( cvImage, cvImage, cv::COLOR_BGR2RGB );
            mpCVImageData = std::make_shared< CVImageData >( cvImage );
            mQPixmap = QPixmap::fromImage( QImage( mpCVImageData->image().data, mpCVImageData->image().cols, mpCVImageData->image().rows, mpCVImageData->image().step, QImage::Format_RGB888 ) );
        }
        mpQLabelImageDisplay->setPixmap( mQPixmap.scaled( miImageDisplayWidth, miImageDisplayHeight, Qt::KeepAspectRatio ) );
        if( mbEnable )
            Q_EMIT dataUpdated( 0 );
    }
}

void
CVImageLoaderModel::
enable_changed( bool enable )
{
    if( enable )
        Q_EMIT dataUpdated( 0 );
}

const QString CVImageLoaderModel::_category = QString( "Source" );

const QString CVImageLoaderModel::_model_name = QString( "CV Image Loader" );
