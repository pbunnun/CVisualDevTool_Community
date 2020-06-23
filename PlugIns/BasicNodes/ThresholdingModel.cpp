#include "ThresholdingModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

ThresholdingModel::
ThresholdingModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":Thresholding.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"Binary", "Threshold Truncated", "Threshold to Zero"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "threshold_type";
    auto propThresholdType = std::make_shared< TypedProperty< EnumPropertyType > >( "Threshold Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propThresholdType );
    mMapIdToProperty[ propId ] = propThresholdType;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdThresholdValue;
    doublePropertyType.mdMax = 255;
    propId = "threshold_value";
    auto propThresholdValue = std::make_shared< TypedProperty< DoublePropertyType > >( "Threshold Value", propId, QVariant::Double, doublePropertyType, "Operation" );
    mvProperty.push_back( propThresholdValue );
    mMapIdToProperty[ propId ] = propThresholdValue;

    doublePropertyType.mdValue = mParams.mdBinaryValue;
    doublePropertyType.mdMax = 255;
    propId = "binary_value";
    auto propBinaryValue = std::make_shared< TypedProperty< DoublePropertyType > >( "Binary Value", propId, QVariant::Double, doublePropertyType , "Operation");
    mvProperty.push_back( propBinaryValue );
    mMapIdToProperty[ propId ] = propBinaryValue;

    propId = "is_inverted";
    auto propIsInverted = std::make_shared< TypedProperty < bool > >("Invert Binary Threshold", propId, QVariant::Bool, mParams.mbIsInverted, "Display");
    mvProperty.push_back( propIsInverted );
    mMapIdToProperty[ propId ] = propIsInverted;
}

unsigned int
ThresholdingModel::
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
ThresholdingModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
ThresholdingModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
ThresholdingModel::
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
ThresholdingModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["thresholdType"] = mParams.miThresholdType;
    cParams["thresholdValue"] = mParams.mdThresholdValue;
    cParams["binaryValue"] = mParams.mdBinaryValue;
    cParams["isInverted"] = mParams.mbIsInverted;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ThresholdingModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "thresholdType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "threshold_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miThresholdType = v.toInt();
        }
        v =  paramsObj[ "thresholdValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "threshold_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdThresholdValue = v.toDouble();
        }
        v =  paramsObj[ "binaryValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "binary_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdBinaryValue = v.toDouble();
        }
        v = paramsObj[ "isInverted" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "is_inverted" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < bool > >(prop);
            typedProp->getData() = v.toBool();

            mParams.mbIsInverted = v.toBool();
        }
    }
}

void
ThresholdingModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "threshold_type" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miThresholdType = 0;
            break;

        case 1:
            mParams.miThresholdType = 2;
            break;

        case 2:
            mParams.miThresholdType = 3;
            break;
        }
    }
    else if( id == "threshold_value" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdThresholdValue = value.toDouble();
    }
    else if( id == "binary_value" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdBinaryValue = value.toDouble();
    }
    else if( id == "is_inverted" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < bool > >(prop);
        typedProp->getData() = value.toBool();

        mParams.mbIsInverted = value.toBool();
        if(mParams.miThresholdType == 0 || mParams.miThresholdType == 1)
        {
            mParams.miThresholdType = mParams.mbIsInverted? 1 : 0 ;
        }
        else if(mParams.miThresholdType == 3 || mParams.miThresholdType == 4)
        {
            mParams.miThresholdType = mParams.mbIsInverted? 4 : 3 ;
        }
    }

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mParams );

        Q_EMIT dataUpdated(0);
    }
}

void
ThresholdingModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> & out,
            const ThresholdingParameters & params )
{
    cv::threshold(in->image(),out->image(),params.mdThresholdValue,params.mdBinaryValue,params.miThresholdType);
}

const QString ThresholdingModel::_category = QString( "Image Operation" );

const QString ThresholdingModel::_model_name = QString( "Thresholding" );
