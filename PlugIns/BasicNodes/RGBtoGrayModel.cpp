#include "RGBtoGrayModel.hpp"

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
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
RGBtoGrayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            processData( d, mpCVImageData );
        }
    }

    Q_EMIT dataUpdated( 0 );
}

void
RGBtoGrayModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > & out )
{
    cv::Mat& in_image = in->image();
    const int& no_channels = in_image.channels();
    if(!in_image.empty())
    {
        if( no_channels == 1 )
        {
            out->set_image( in_image );
        }
        else if( no_channels == 3 )
        {
            cv::cvtColor( in_image, out->image(), cv::COLOR_BGR2GRAY );
        }
    }
}

const QString RGBtoGrayModel::_category = QString("Image Conversion");

const QString RGBtoGrayModel::_model_name = QString( "RGB to Gray" );
