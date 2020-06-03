#include "Filter2DModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>

Filter2DModel::
Filter2DModel()
    : PBNodeDataModel( _model_name ),
      _minPixmap( ":RGBtoGray.png" )
{

}

unsigned int
Filter2DModel::
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
Filter2DModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr< NodeData >
Filter2DModel::
outData(PortIndex)
{
    return mpCVImageData;
}


void
Filter2DModel::
setInData(std::shared_ptr< NodeData > nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast< CVImageData >(nodeData);
        if (d)
        {
            if( d->image().channels() != 1 )
            {
                cv::Mat cvGray;
                cv::cvtColor(d->image(), cvGray, cv::COLOR_RGB2GRAY);
                mpCVImageData = std::make_shared< CVImageData >(cvGray);
            }
        }
    }

    Q_EMIT dataUpdated(0);
}

const QString Filter2DModel::_category = QString( "Image Operation" );

const QString Filter2DModel::_model_name = QString( "2D Filter" );
