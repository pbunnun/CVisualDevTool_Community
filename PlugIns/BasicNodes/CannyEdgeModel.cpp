#include "CannyEdgeModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

CannyEdgeModel::
CannyEdgeModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":CannyEdge.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    IntPropertyType intPropertyType;
    QString propId = "kernel_size";
    intPropertyType.miValue = mParams.miSizeKernel;
    auto propKernelSize = std::make_shared< TypedProperty< IntPropertyType > >( "Kernel Size", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propKernelSize );
    mMapIdToProperty[ propId ] = propKernelSize;

    intPropertyType.miValue = mParams.miThresholdU;
    intPropertyType.miMax = 255;
    propId = "th_u";
    auto propThresholdU = std::make_shared< TypedProperty< IntPropertyType > >( "Upper Threshold", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propThresholdU );
    mMapIdToProperty[ propId ] = propThresholdU;

    intPropertyType.miValue = mParams.miThresholdL;
    propId = "th_l";
    auto propThresholdL = std::make_shared< TypedProperty< IntPropertyType > >( "Lower Threshold", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propThresholdL );
    mMapIdToProperty[ propId ] = propThresholdL;
}

unsigned int
CannyEdgeModel::
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
CannyEdgeModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
CannyEdgeModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
CannyEdgeModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            processData( mpCVImageInData, mpCVImageData, mParams );
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
CannyEdgeModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["kernelSize"] = mParams.miSizeKernel;
    cParams["thresholdU"] = mParams.miThresholdU;
    cParams["thresholdL"] = mParams.miThresholdL;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
CannyEdgeModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "kernelSize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "kernel_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miSizeKernel = v.toInt();
        }
        v =  paramsObj[ "thresholdU" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "th_u" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miThresholdU = v.toInt();
        }
        v =  paramsObj[ "thresholdL" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "th_l" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miThresholdL = v.toInt();
        }
    }
}

void
CannyEdgeModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "kernel_size" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        int kSize =  value.toInt();
        bool adjValue = false;
        if( kSize > 7 )
        {
            kSize = 7;
            adjValue = true;
        }
        if( kSize < 3 )
        {
            kSize = 3;
            adjValue = true;
        }
        if( kSize >=3 && kSize <=7 && kSize%2!=1 )
        {
            kSize += 1;
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miValue = kSize;

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = kSize;

            mParams.miSizeKernel = kSize;
        }
    }
    else if( id == "th_u" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miThresholdU = value.toInt();
    }
    else if( id == "th_l" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miThresholdL = value.toInt();
    }

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mParams );

        Q_EMIT dataUpdated(0);
    }
}

void
CannyEdgeModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> & out,
            const CannyEdgeParameters & params )
{
    cv::Canny(in->image(), out->image(), params.miThresholdL, params.miThresholdU, params.miSizeKernel );
}

const QString CannyEdgeModel::_category = QString( "Image Operation" );

const QString CannyEdgeModel::_model_name = QString( "Canny Edge" );
