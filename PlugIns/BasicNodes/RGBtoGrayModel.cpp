#include "RGBtoGrayModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>

RGBtoGrayModel::
RGBtoGrayModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":RGBtoGray.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
}

unsigned int
RGBtoGrayModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;
        break;

    default:
        break;
    }

    return result;
}

NodeDataType
RGBtoGrayModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}

std::shared_ptr<NodeData>
RGBtoGrayModel::
outData(PortIndex)
{
    if( mbEnable && mpCVImageData->image().data != nullptr )
        return mpCVImageData;
    else
        return nullptr;
}

void
RGBtoGrayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !mbEnable )
        return;

    if( nodeData )
    {
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            if( d->image().channels() != 1 )
            {
                cv::Mat cvGrayImage;
                cv::cvtColor( d->image(), cvGrayImage, cv::COLOR_RGB2GRAY );
                mpCVImageData->set_image( cvGrayImage );
            }
        }
    }

    Q_EMIT dataUpdated( 0 );
}

const QString RGBtoGrayModel::_category = QString("Image Operation");

const QString RGBtoGrayModel::_model_name = QString( "RGB to Gray" );
