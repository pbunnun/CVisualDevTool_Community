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
            cv::Mat RGBtoGrayImage = processData(d);
            mpCVImageData = std::make_shared<CVImageData>(RGBtoGrayImage);
        }
    }

    Q_EMIT dataUpdated( 0 );
}

cv::Mat RGBtoGrayModel::processData(const std::shared_ptr<CVImageData> &p)
{
    cv::Mat Output;
    int no_channels = p->image().channels();
    if( no_channels == 1 )
    {
        Output = p->image().clone();
    }
    else if( no_channels == 3 )
    {
        cv::cvtColor( p->image(), Output, cv::COLOR_BGR2GRAY );
    }
    return Output;
}

const QString RGBtoGrayModel::_category = QString("Image Operation");

const QString RGBtoGrayModel::_model_name = QString( "RGB to Gray" );
