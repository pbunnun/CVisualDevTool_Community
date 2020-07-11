#include "CornerDetectionModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

CornerDetectionModel::
CornerDetectionModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new CornerDetectionEmbeddedWidget ),
      _minPixmap( ":CornerDetection.png" )
{
    mpStdVectorData_CVPoint = std::make_shared< StdVectorData<cv::Point> >(std::vector<cv::Point>());
    mpSyncData = std::make_shared< SyncData >();

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miMaxCorners;
    QString propId = "max_corners";
    auto propMaxCorners = std::make_shared< TypedProperty< IntPropertyType > >( "Max Corners", propId, QVariant::Int, intPropertyType, "Operation");
    mvProperty.push_back( propMaxCorners );
    mMapIdToProperty[ propId ] = propMaxCorners;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdQualityLevel;
    propId = "quality_level";
    auto propQualityLevel = std::make_shared< TypedProperty< DoublePropertyType > >( "Quality Level", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propQualityLevel );
    mMapIdToProperty[ propId ] = propQualityLevel;

    doublePropertyType.mdValue = mParams.mdMinDistance;
    propId = "min_distance";
    auto propMinDistance = std::make_shared< TypedProperty< DoublePropertyType > >( "Minimum Distance", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propMinDistance );
    mMapIdToProperty[ propId ] = propMinDistance;

    intPropertyType.miValue = mParams.miBlockSize;
    propId = "block_size";
    auto propBlockSize = std::make_shared< TypedProperty< IntPropertyType > >( "Block Size", propId, QVariant::Int, intPropertyType, "Operation" );
    mvProperty.push_back( propBlockSize );
    mMapIdToProperty[ propId ] = propBlockSize;

    intPropertyType.miValue = mParams.miGradientSize;
    propId = "gradient_size";
    auto propGradientSize = std::make_shared< TypedProperty< IntPropertyType > >( "Gradient Size", propId, QVariant::Int, intPropertyType , "Operation");
    mvProperty.push_back( propGradientSize );
    mMapIdToProperty[ propId ] = propGradientSize;

    propId = "use_harris";
    auto propUseHarris = std::make_shared< TypedProperty < bool > > ("Use Harris", propId, QVariant::Bool, mParams.mbUseHarris, "Operation");
    mvProperty.push_back( propUseHarris );
    mMapIdToProperty[ propId ] = propUseHarris;

    doublePropertyType.mdValue = mParams.mdFreeParameter;
    propId = "free_parameter";
    auto propFreeParameter = std::make_shared< TypedProperty< DoublePropertyType > >( "Free Parameter", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propFreeParameter );
    mMapIdToProperty[ propId ] = propFreeParameter;

    propId = "active_mask";
    auto propActiveMask = std::make_shared< TypedProperty < bool > > ("", propId, QVariant::Bool, mpEmbeddedWidget->get_maskStatus_label());
    mMapIdToProperty[ propId ] = propActiveMask;
}

unsigned int
CornerDetectionModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
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
CornerDetectionModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::Out)
    {
        if(portIndex == 0)
            return StdVectorData<cv::Point>().type();
        if(portIndex == 1)
            return SyncData().type();
    }
    return CVImageData().type();
}


std::shared_ptr<NodeData>
CornerDetectionModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I == 0)
        {
            return mpStdVectorData_CVPoint;
        }
        else if(I == 1)
        {
            return mpSyncData;
        }
    }
    return nullptr;
}

void
CornerDetectionModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        mpSyncData->state() = false;
        Q_EMIT dataUpdated(1);
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mapCVImageInData[portIndex] = d;
            if( mapCVImageInData[0] )
            {
                processData( mapCVImageInData, mpStdVectorData_CVPoint, mParams );
            }
        }
        mpSyncData->state() = true;
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
CornerDetectionModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["maxCorners"] = mParams.miMaxCorners;
    cParams["qualityLevel"] = mParams.mdQualityLevel;
    cParams["minDistance"] = mParams.mdMinDistance;
    cParams["blockSize"] = mParams.miBlockSize;
    cParams["gradientSize"] = mParams.miGradientSize;
    cParams["useHarris"] = mParams.mbUseHarris;
    cParams["freeParameter"] = mParams.mdFreeParameter;
    cParams["activeMask"] = mpEmbeddedWidget->get_maskStatus_label();
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
CornerDetectionModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "maxCorners" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "max_corners" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miMaxCorners = v.toInt();
        }
        v =  paramsObj[ "qualityLevel" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "quality_level" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdQualityLevel = v.toDouble();
        }
        v =  paramsObj[ "minDistance" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "min_distance" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdMinDistance = v.toDouble();
        }
        v =  paramsObj[ "blockSize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "block_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miBlockSize = v.toInt();
        }
        v =  paramsObj[ "gradientSize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "gradient_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miGradientSize = v.toInt();
        }
        v = paramsObj[ "useHarris" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "use_harris" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < bool > > (prop);
            typedProp->getData() = v.toBool();

            mParams.mbUseHarris = v.toBool();
        }
        v =  paramsObj[ "freeParameter" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "free_parameter" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdFreeParameter = v.toDouble();
        }
        v = paramsObj[ "activeMask" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "active_mask" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mpEmbeddedWidget->set_maskStatus_label(v.toBool());
        }
    }
}

void
CornerDetectionModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->state() = false;
    Q_EMIT dataUpdated(1);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "max_corners" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miMaxCorners = value.toInt();
    }
    else if( id == "quality_level" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdQualityLevel = value.toDouble();
    }
    else if( id == "min_distance" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdMinDistance = value.toDouble();
    }
    else if( id == "block_size" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        int bSize =  value.toInt();
        bool adjValue = false;
        if( bSize % 2 != 1)
        {
            bSize += bSize == typedProp->getData().miMax? -1 : 1;
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miValue = bSize;

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = bSize;

            mParams.miBlockSize = bSize;
        }
    }
    else if( id == "gradient_size" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        int gSize =  value.toInt();
        bool adjValue = false;
        if( gSize % 2 != 1)
        {
            gSize += gSize == typedProp->getData().miMax? -1 : 1;
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miValue = gSize;

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = gSize;

            mParams.miBlockSize = gSize;
        }
    }
    else if( id == "use_harris" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbUseHarris = value.toBool();
    }
    else if( id == "free_parameter" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdFreeParameter = value.toDouble();
    }

    if( mapCVImageInData[0] )
    {
        processData( mapCVImageInData, mpStdVectorData_CVPoint, mParams );

        Q_EMIT dataUpdated(0);
    }
    mpSyncData->state() = true;
    Q_EMIT dataUpdated(1);
}

void
CornerDetectionModel::
processData(const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr<StdVectorData<cv::Point>> & out,
            const CornerDetectionParameters & params )
{
    const cv::Mat& in_image = in[0]->image();
    if(in_image.empty() || (in_image.type()!=CV_8UC1 && in_image.type()!=CV_8SC1 && in_image.type()!=CV_32FC1))
    {
        return;
    }
    const bool activeMask = in[1]!=nullptr && !in[1]->image().empty() && in[1]->image().type()==CV_8UC1 &&
    in[1]->image().rows == in_image.rows && in[1]->image().cols == in_image.cols;
    mpEmbeddedWidget->set_maskStatus_label(activeMask);
    if(activeMask)
    {
        cv::goodFeaturesToTrack(in_image,
                                out->vector(),
                                params.miMaxCorners,
                                params.mdQualityLevel,
                                params.mdMinDistance,
                                in[1]->image(),
                                params.miBlockSize,
                                params.miGradientSize,
                                params.mbUseHarris,
                                params.mdFreeParameter);
    }
    else
    {
        cv::goodFeaturesToTrack(in_image,
                                out->vector(),
                                params.miMaxCorners,
                                params.mdQualityLevel,
                                params.mdMinDistance,
                                cv::Mat(),
                                params.miBlockSize,
                                params.miGradientSize,
                                params.mbUseHarris,
                                params.mdFreeParameter);
    }
}

const std::string CornerDetectionModel::color[3] = {"B","G","R"};

const QString CornerDetectionModel::_category = QString( "Image Processing" );

const QString CornerDetectionModel::_model_name = QString( "Corner Detection" );

