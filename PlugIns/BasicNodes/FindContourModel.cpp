#include "FindContourModel.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <QEvent>
#include <QDir>
#include "qtvariantproperty.h"

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>


FindContourModel::
FindContourModel()
    : PBNodeDataModel( _model_name, true),
      _minPixmap(":FindContour.png")
{
    mpStdVectorData_StdVector_CVPoint = std::make_shared< StdVectorData<std::vector<cv::Point>> >();
    mpStdVectorData_CVVec4i = std::make_shared< StdVectorData<cv::Vec4i> >( std::vector<cv::Vec4i>() );
    mpSyncData = std::make_shared< SyncData >();

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"RETR_LIST","RETR_TREE","RETR_CCOMP","RETR_EXTERNAL","RETR_FLOODFILL"});
    enumPropertyType.miCurrentIndex = 1; //initializing this to 0 somehow produces an unexpected output
    QString propId = "contour_mode";
    auto propContourMode = std::make_shared< TypedProperty< EnumPropertyType > >("Contour Mode", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propContourMode );
    mMapIdToProperty[ propId ] = propContourMode;

    enumPropertyType.mslEnumNames = QStringList( {"CHAIN_APPROX_NONE", "CHAIN_APPROX_SIMPLE", "CHAIN_APPROX_TC89_L1","CHAIN_APPROX_TC89_KCOS"} );
    enumPropertyType.miCurrentIndex = 1; //initializing this to 0 somehow produces an unexpected output
    propId = "contour_method";
    auto propContourMethod = std::make_shared< TypedProperty< EnumPropertyType > >( "Contour Method", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propContourMethod );
    mMapIdToProperty[ propId ] = propContourMethod;
}

unsigned int
FindContourModel::
nPorts( PortType portType ) const
{
    switch( portType )
    {
    case PortType::In:
        return( 1 );
    case PortType::Out:
        return( 3 );
    default:
        return( -1 );
    }
}

NodeDataType
FindContourModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return CVImageData().type();
    }
    else if(portType == PortType::Out)
    {
        if(portIndex==0)
            return StdVectorData<std::vector<cv::Point>>().type();
        else if(portIndex == 1)
            return StdVectorData<cv::Vec4i>().type();
        else if(portIndex == 2)
            return SyncData().type();
    }
    return NodeDataType();
}

std::shared_ptr<NodeData>
FindContourModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I == 0)
        {
            return mpStdVectorData_StdVector_CVPoint;
        }
        else if(I == 1)
        {
            return mpStdVectorData_CVVec4i;
        }
        else if(I == 2)
        {
            return mpSyncData;
        }
    }
    return nullptr;
}

void
FindContourModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex)
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(2);
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            mpCVImageInData = d;
            processData(mpCVImageInData,mpStdVectorData_StdVector_CVPoint,
                        mpStdVectorData_CVVec4i, mParams);
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(2);
    }
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
}

QJsonObject
FindContourModel::
save() const
{
    /*
     * If save() was overrided, PBNodeDataModel::save() must be called explicitely.
     */
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams[ "contourMode" ] = mParams.miContourMode;
    cParams[ "contourMethod" ] = mParams.miContourMethod;
    modelJson[ "cParams" ] = cParams;

    return modelJson;
}

void
FindContourModel::
restore(const QJsonObject &p)
{
    /*
     * If restore() was overrided, PBNodeDataModel::restore() must be called explicitely.
     */
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "contourMode" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "contour_mode" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miContourMode = v.toInt();

        }
        v = paramsObj[ "contourMethod" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "contour_method" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miContourMethod = v.toInt();
        }
    }


}

void
FindContourModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(2);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "contour_mode" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        switch(value.toInt())
        { //{"RETR_LIST","RETR_TREE","RETR_CCOMP","RETR_EXTERNAL","RETR_FLOODFILL"}
        case 0:
            mParams.miContourMode = cv::RETR_LIST;
            break;

        case 1:
            mParams.miContourMode = cv::RETR_TREE;
            break;

        case 2:
            mParams.miContourMode = cv::RETR_CCOMP;
            break;

        case 3:
            mParams.miContourMode = cv::RETR_EXTERNAL;
            break;

        case 4: //produces a bug when this case is executed
            mParams.miContourMode = cv::RETR_FLOODFILL;
            break;
        }
    }
    else if( id == "contour_method" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        switch(value.toInt())
        {
        case 0:
            mParams.miContourMethod = cv::CHAIN_APPROX_NONE;
            break;

        case 1:
            mParams.miContourMethod = cv::CHAIN_APPROX_SIMPLE;
            break;

        case 2:
            mParams.miContourMethod = cv::CHAIN_APPROX_TC89_L1;
            break;

        case 3:
            mParams.miContourMethod = cv::CHAIN_APPROX_TC89_KCOS;
            break;
        }
    }

    if(mpCVImageInData)
    {
        processData(mpCVImageInData,mpStdVectorData_StdVector_CVPoint,
                    mpStdVectorData_CVVec4i,mParams);

        Q_EMIT dataUpdated(0);
        Q_EMIT dataUpdated(1);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(2);
}

void FindContourModel::processData(const std::shared_ptr<CVImageData> &in, std::shared_ptr<StdVectorData<std::vector<cv::Point>>> &outVecVec,
                                   std::shared_ptr<StdVectorData<cv::Vec4i>> &outVec, const FindContourParameters &params)
{
    const cv::Mat& in_image = in->image();
    if(in_image.empty() || (in_image.type()!=CV_8UC1 && in_image.type()!=CV_8SC1))
    {
        return;
    }
    cv::findContours(in_image,
                     outVecVec->vector(),
                     outVec->vector(),
                     params.miContourMode,
                     params.miContourMethod);
}

const QString FindContourModel::_category = QString( "Image Processing" );

const QString FindContourModel::_model_name = QString( "Find Contour" );
