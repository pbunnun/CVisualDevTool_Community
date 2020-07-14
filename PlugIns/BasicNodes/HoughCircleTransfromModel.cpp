#include "HoughCircleTransfromModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "qtvariantproperty.h"

HoughCircleTransformModel::
HoughCircleTransformModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":HoughCircleTransform.png" )
{
    mpStdVectorData_CVVec3f = std::make_shared< StdVectorData<cv::Vec3f> >( std::vector<cv::Vec3f>() );
    mpIntegerData = std::make_shared< IntegerData >( int() );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"HOUGH_GRADIENT", "HOUGH_STANDARD", "HOUGH_MULTI_SCALE", "HOUGH_GRADIENT_ALT", "HOUGH_PROBABILISTIC"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "hough_method";
    auto propHoughMethod = std::make_shared< TypedProperty< EnumPropertyType > >( "Method", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propHoughMethod );
    mMapIdToProperty[ propId ] = propHoughMethod;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdInverseRatio;
    propId = "inverse_ratio";
    auto propInverseRatio = std::make_shared< TypedProperty< DoublePropertyType > >( "Resolution Inverse Ratio", propId, QVariant::Double, doublePropertyType, "Operation" );
    mvProperty.push_back( propInverseRatio );
    mMapIdToProperty[ propId ] = propInverseRatio;

    doublePropertyType.mdValue = mParams.mdCenterDistance;
    propId = "center_distance";
    auto propCenterDistance = std::make_shared< TypedProperty< DoublePropertyType > >( "Minimum Center Distance", propId, QVariant::Double, doublePropertyType , "Operation");
    mvProperty.push_back( propCenterDistance );
    mMapIdToProperty[ propId ] = propCenterDistance;

    doublePropertyType.mdValue = mParams.mdThresholdU;
    propId = "th_u";
    auto propThresholdU = std::make_shared< TypedProperty< DoublePropertyType > >( "Upper Threshold", propId, QVariant::Double, doublePropertyType , "Operation");
    mvProperty.push_back( propThresholdU );
    mMapIdToProperty[ propId ] = propThresholdU;

    doublePropertyType.mdValue = mParams.mdThresholdL;
    propId = "th_l";
    auto propThresholdL = std::make_shared< TypedProperty< DoublePropertyType > >( "Lower Threshold", propId, QVariant::Double, doublePropertyType , "Operation");
    mvProperty.push_back( propThresholdL );
    mMapIdToProperty[ propId ] = propThresholdL;

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miRadiusMin;
    propId = "radius_min";
    auto propRadiusMin = std::make_shared<TypedProperty<IntPropertyType>>("Minimum Radius", propId, QVariant::Int, intPropertyType, "Operation");
    mvProperty.push_back(propRadiusMin);
    mMapIdToProperty[ propId ] = propRadiusMin;

    intPropertyType.miValue = mParams.miRadiusMax;
    intPropertyType.miMin = -1;
    propId = "radius_max";
    auto propRadiusMax = std::make_shared<TypedProperty<IntPropertyType>>("Maximum Radius", propId, QVariant::Int, intPropertyType, "Operation");
    mvProperty.push_back(propRadiusMax);
    mMapIdToProperty[ propId ] = propRadiusMax;
}

unsigned int
HoughCircleTransformModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 2;
        break;

    default:
        break;
    }

    return result;
}


NodeDataType
HoughCircleTransformModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return CVImageData().type();
    }
    else if(portType == PortType::Out)
    {
        if(portIndex == 0)
            return StdVectorData<cv::Vec3f>().type();
        else if(portIndex == 1)
            return IntegerData().type();
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
HoughCircleTransformModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I == 0)
        {
            return mpStdVectorData_CVVec3f;
        }
        else if(I == 1)
        {
            return mpIntegerData;
        }
    }
    return nullptr;
}

void
HoughCircleTransformModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            processData( mpCVImageInData, mpStdVectorData_CVVec3f, mpIntegerData, mParams);
        }
    }

    updateAllOutputPorts();
}

QJsonObject
HoughCircleTransformModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["houghMethod"] = mParams.miHoughMethod;
    cParams["inverseRatio"] = mParams.mdInverseRatio;
    cParams["centerDistance"] = mParams.mdCenterDistance;
    cParams["thresholdU"] = mParams.mdThresholdU;
    cParams["thresholdL"] = mParams.mdThresholdL;
    cParams["radiusMin"] = mParams.miRadiusMin;
    cParams["radiusMax"] = mParams.miRadiusMax;
    return modelJson;
}

void
HoughCircleTransformModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "houghMethod" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "hough_method" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miHoughMethod = v.toInt();
        }
        v = paramsObj[ "inverseRatio" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "inverse_ratio" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdInverseRatio = v.toDouble();
        }
        v = paramsObj[ "centerDistance" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "center_distance" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdCenterDistance = v.toDouble();
        }
        v =  paramsObj[ "thresholdU" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "th_u" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdThresholdU = v.toDouble();
        }
        v =  paramsObj[ "thresholdL" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "th_l" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdThresholdL = v.toDouble();
        }
        v =  paramsObj[ "radiusMin" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "radius_min" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miRadiusMin = v.toInt();
        }
        v =  paramsObj[ "radiusMax" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "radius_max" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miRadiusMax = v.toInt();
        }
    }
}

void
HoughCircleTransformModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "hough_method" )
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<EnumPropertyType>>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miHoughMethod = cv::HOUGH_GRADIENT;
            break;

        case 1:
            mParams.miHoughMethod = cv::HOUGH_STANDARD;
            break;

        case 2:
            mParams.miHoughMethod = cv::HOUGH_MULTI_SCALE;
            break;

        case 3:
            mParams.miHoughMethod = cv::HOUGH_GRADIENT_ALT;
            break;

        case 4:
            mParams.miHoughMethod = cv::HOUGH_PROBABILISTIC;
            break;
        }
    }
    else if( id == "inverse_ratio" )
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<DoublePropertyType>>(prop);
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdInverseRatio = value.toDouble();
    }
    else if( id == "center_distance" )
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<DoublePropertyType>>(prop);
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdCenterDistance = value.toDouble();
    }
    else if( id == "th_u" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdThresholdU = value.toDouble();
    }
    else if( id == "th_l" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdThresholdL = value.toDouble();
    }
    else if( id == "radius_min" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miRadiusMin = value.toInt();
    }
    else if( id == "radius_max" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miRadiusMax = value.toInt();
    }

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpStdVectorData_CVVec3f, mpIntegerData, mParams);

        updateAllOutputPorts();
    }
}

void
HoughCircleTransformModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<StdVectorData<cv::Vec3f>> & out,
            std::shared_ptr<IntegerData> &outInt, const HoughCircleTransformParameters & params)
{
    cv::Mat& in_image = in->image();
    if(in_image.empty() || in_image.type()!=CV_8UC1)
    {
        return;
    }
    cv::HoughCircles(in_image,
                     out->vector(),
                     params.miHoughMethod,
                     params.mdInverseRatio,
                     params.mdCenterDistance,
                     params.mdThresholdU,
                     params.mdThresholdL,
                     params.miRadiusMin,
                     params.miRadiusMax);
    outInt->number() = static_cast<int>(out->vector().size());
}

const std::string HoughCircleTransformModel::color[3] = {"B", "G", "R"};

const QString HoughCircleTransformModel::_category = QString( "Image Processing" );

const QString HoughCircleTransformModel::_model_name = QString( "Hough Circle" );
