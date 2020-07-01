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
    enumPropertyType.mslEnumNames = QStringList({"THRESH_MASK", "THRESH_OTSU", "THRESH_TRUNC", "THRESH_BINARY", "THRESH_TOZERO", "THRESH_TRIANGLE", "THRESH_BINARY_INV", "THRESH_TOZERO_INV", "ADAPTIVE_THRESH_MEAN_C", "ADAPTIVE_THRESH_GAUSSIAN_C"});
    enumPropertyType.miCurrentIndex = 3;
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

    propId = "otsu_threshold";
    auto propOtsuThreshold = std::make_shared<TypedProperty< QString > >("Otsu Threshold", propId, QVariant::String, QString("%2").arg(mProps.mdOtsuThreshold), "Properties");
    mvProperty.push_back(( propOtsuThreshold ));
    mMapIdToProperty[ propId ] = propOtsuThreshold;
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
            processData( mpCVImageInData, mpCVImageData, mParams, mProps );
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
    cParams["otsuThreshold"] = QString("%2").arg(mProps.mdOtsuThreshold);
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
        v = paramsObj[ "otsuThreshold" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "otsu_threshold" ];
            auto typedProp = std::static_pointer_cast< TypedProperty <QString>>(prop);
            typedProp->getData() = v.toString();

            mProps.mdOtsuThreshold = v.toDouble();
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
            mParams.miThresholdType = cv::THRESH_MASK;
            break;

        case 1:
            mParams.miThresholdType = cv::THRESH_OTSU;
            break;

        case 2:
            mParams.miThresholdType = cv::THRESH_TRUNC;
            break;

        case 3:
            mParams.miThresholdType = cv::THRESH_BINARY;
            break;

        case 4:
            mParams.miThresholdType = cv::THRESH_TOZERO;
            break;

        case 5:
            mParams.miThresholdType = cv::THRESH_TRIANGLE;
            break;

        case 6:
            mParams.miThresholdType = cv::THRESH_BINARY_INV;
            break;

        case 7:
            mParams.miThresholdType = cv::THRESH_TOZERO_INV;
            break;

        case 8:
            mParams.miThresholdType = cv::ADAPTIVE_THRESH_MEAN_C;
            break;

        case 9:
            mParams.miThresholdType = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
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

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mParams, mProps );
        Q_EMIT dataUpdated(0);
    }
}

void
ThresholdingModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> & out,
            const ThresholdingParameters & params, ThresholdingProperties &props)
{
    if(in->image().channels()==1)
    {
        if(params.miThresholdType == cv::THRESH_OTSU)
        {
            props.mdOtsuThreshold = cv::threshold(in->image(),out->image(),params.mdThresholdValue,params.mdBinaryValue,params.miThresholdType);
        }
        else
        {
            cv::threshold(in->image(),out->image(),params.mdThresholdValue,params.mdBinaryValue,params.miThresholdType);
            props.mdOtsuThreshold = 0;
        }
    }

    auto prop = mMapIdToProperty["otsu_threshold"];
    auto typedProp = std::static_pointer_cast<TypedProperty<QString>>(prop);
    typedProp->getData() = QString("%2").arg(props.mdOtsuThreshold);
}

const QString ThresholdingModel::_category = QString( "Image Conversion" );

const QString ThresholdingModel::_model_name = QString( "Thresholding" );
