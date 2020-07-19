#include "ConvexHullModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

ConvexHullModel::
ConvexHullModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":ConvexHull.png" )
{
    //std::make_shared below must be called in processData
    mpStdVectorData_StdVector_CVPoint = std::make_shared< StdVectorData<std::vector<cv::Point>>>( std::vector<std::vector<cv::Point>>() );
    mpSyncData = std::make_shared< SyncData >();

    QString propId = "clockwise";
    auto propClockwise = std::make_shared< TypedProperty< bool > >( "Clockwise", propId, QVariant::Bool, mParams.mbClockwise, "Operation");
    mvProperty.push_back( propClockwise );
    mMapIdToProperty[ propId ] = propClockwise;

    propId = "return_points";
    auto propReturnPoints = std::make_shared< TypedProperty< bool > >( "Return Points", propId, QVariant::Bool, mParams.mbReturnPoints, "Operation");
    mvProperty.push_back( propReturnPoints );
    mMapIdToProperty[ propId ] = propReturnPoints;
}

unsigned int
ConvexHullModel::
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
ConvexHullModel::
dataType(PortType, PortIndex portIndex) const
{
    if(portIndex == 0)
        return StdVectorData<std::vector<cv::Point>>().type();
    else
        return SyncData().type();
}


std::shared_ptr<NodeData>
ConvexHullModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if( I == 0 )
            return mpStdVectorData_StdVector_CVPoint;
        else if( I == 1 )
            return mpSyncData;
    }
    return nullptr;
}

void
ConvexHullModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if(nodeData)
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        auto d = std::dynamic_pointer_cast<StdVectorData<std::vector<cv::Point>>>(nodeData);
        if( d )
        {
            mpStdVectorInData_StdVector_CVPoint = d;
            processData(mpStdVectorInData_StdVector_CVPoint,
                        mpStdVectorData_StdVector_CVPoint,
                        mParams);
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
ConvexHullModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["clockwise"] = mParams.mbClockwise;
    cParams["returnPoints"] = mParams.mbReturnPoints;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ConvexHullModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "clockwise" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "clockwise" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            mParams.mbClockwise = v.toBool();
        }
        v = paramsObj[ "returnPoints" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "return_points" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            mParams.mbReturnPoints = v.toBool();
        }
    }
}

void
ConvexHullModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "clockwise" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty <bool>>(prop);
        typedProp->getData() = value.toBool();

        mParams.mbClockwise = value.toBool();
    }
    else if( id == "return_points" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty <bool>>(prop);
        typedProp->getData() = value.toBool();

        mParams.mbReturnPoints = value.toBool();
    }

    if( mpStdVectorInData_StdVector_CVPoint )
    {
        processData( mpStdVectorInData_StdVector_CVPoint,
                     mpStdVectorData_StdVector_CVPoint,
                     mParams);

        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
ConvexHullModel::
processData( const std::shared_ptr<StdVectorData<std::vector<cv::Point>>> & in,
             std::shared_ptr<StdVectorData<std::vector<cv::Point>>> & out,
             const ConvexHullParameters & params )
{
    const auto& in_vector = in->vector();
    const size_t size = in_vector.size();
    auto& out_vector = out->vector();
    out_vector.resize(size);
    for(size_t i=0; i<size; i++)
    {
        cv::convexHull(in_vector[i],
                       out_vector[i],
                       params.mbClockwise,
                       params.mbReturnPoints);
    }
}


const QString ConvexHullModel::_category = QString( "Image Processing" );

const QString ConvexHullModel::_model_name = QString( "Convex Hull" );

