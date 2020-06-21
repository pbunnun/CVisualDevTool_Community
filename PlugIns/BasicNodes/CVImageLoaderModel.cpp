#include "CVImageLoaderModel.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include "qtvariantproperty.h"
#include <QMessageBox>

CVImageLoaderModel::
CVImageLoaderModel()
    : PBNodeDataModel( _model_name, true ),
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
        return( 0 );
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
    if( isEnable() && portIndex == 0 && mpCVImageData->image().data != nullptr )
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
    if( !QFile::exists( msImageFilename ) )
        return;

    QImage qImage = QImage( msImageFilename );

    if( qImage.isNull() )
    {
        QMessageBox msg;
        QString msgText = "Cannot load this image!";
        msg.setIcon( QMessageBox::Critical );
        msg.setText( msgText );
        msg.exec();
        return; // unsupport image formats
    }

    auto q_image_format = qImage.format();
    int cv_image_format = 0;

    if( q_image_format == QImage::Format_Grayscale8 )
    {
        cv_image_format = CV_8UC1;
    }
    else if( q_image_format == QImage::Format_Grayscale16 || q_image_format == QImage::Format_Mono || q_image_format == QImage::Format_MonoLSB )
    {
        qImage.convertTo( QImage::Format_Grayscale8 );
        cv_image_format = CV_8UC1;
    }
    else if( q_image_format == QImage::Format_Invalid || q_image_format == QImage::Format_Alpha8 )
    {
        QMessageBox msg;
        QString msgText = "Image format is not supported!";
        msg.setIcon( QMessageBox::Critical );
        msg.setText( msgText );
        msg.exec();
        return; // unsupport image formats
    }
    else
    {
        qImage.convertTo( QImage::Format_BGR888 );
        cv_image_format = CV_8UC3;
    }

    cv::Mat cvImage = cv::Mat( qImage.height(), qImage.width(), cv_image_format, const_cast<uchar*>( qImage.bits() ), static_cast<size_t>(qImage.bytesPerLine()) );

    if( cvImage.data != nullptr )
    {
        mpCVImageData = std::make_shared< CVImageData >( cvImage );

        mQPixmap = QPixmap::fromImage( qImage );

        mpQLabelImageDisplay->setPixmap( mQPixmap.scaled( miImageDisplayWidth, miImageDisplayHeight, Qt::KeepAspectRatio ) );
        if( isEnable() )
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
