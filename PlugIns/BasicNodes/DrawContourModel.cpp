#include "DrawContourModel.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <QEvent>
#include <QDir>
#include "qtvariantproperty.h"

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>


DrawContourModel::
DrawContourModel()
    : PBNodeDataModel( _model_name, true),
      _minPixmap(":DrawContour.png")
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

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

    UcharPropertyType ucharPropertyType;
    ucharPropertyType.mucValue = mParams.mucBValue;
    propId = "b_value";
    auto propBValue = std::make_shared<TypedProperty<UcharPropertyType>>("B value",propId,QVariant::Int,ucharPropertyType, "Operation");
    mvProperty.push_back(propBValue);
    mMapIdToProperty[propId] = propBValue;

    ucharPropertyType.mucValue = mParams.mucGValue;
    propId = "g_value";
    auto propGValue = std::make_shared<TypedProperty<UcharPropertyType>>("G value",propId,QVariant::Int,ucharPropertyType, "Operation");
    mvProperty.push_back(propGValue);
    mMapIdToProperty[propId] = propGValue;

    ucharPropertyType.mucValue = mParams.mucRValue;
    propId = "r_value";
    auto propRValue = std::make_shared<TypedProperty<UcharPropertyType>>("R value",propId,QVariant::Int,ucharPropertyType, "Operation");
    mvProperty.push_back(propRValue);
    mMapIdToProperty[propId] = propRValue;

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared<TypedProperty<IntPropertyType>>("Line Thickness",propId,QVariant::Int,intPropertyType, "Display");
    mvProperty.push_back(propLineThickness);
    mMapIdToProperty[propId] = propLineThickness;

    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "line_type";
    auto propLineType = std::make_shared<TypedProperty<EnumPropertyType>>("Line Type",propId,QtVariantPropertyManager::enumTypeId(),enumPropertyType, "Display");
    mvProperty.push_back(propLineType);
    mMapIdToProperty[propId] = propLineType;

    propId = "contour_count";
    auto propContourCount = std::make_shared<TypedProperty<QString>>("Contour Count",propId,QVariant::String,QString("%1").arg(mProps.miContourCount), "Properties");
    mvProperty.push_back(propContourCount);
    mMapIdToProperty[propId] = propContourCount;
}

unsigned int
DrawContourModel::
nPorts( PortType portType ) const
{
    switch( portType )
    {
    case PortType::In:
        return( 1 );
    case PortType::Out:
        return( 1 );
    default:
        return( -1 );
    }
}

NodeDataType
DrawContourModel::
dataType(PortType , PortIndex) const
{
    return CVImageData().type();
}

std::shared_ptr<NodeData>
DrawContourModel::
outData(PortIndex)
{
    if( isEnable() )
    {
        return mpCVImageData;
    }
    else
    {
        return nullptr;
    }
}

void
DrawContourModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            mpCVImageInData = d;
            processData(mpCVImageInData,mpCVImageData,mParams,mProps);
        }
    }
    Q_EMIT dataUpdated( 0 );
}

QJsonObject
DrawContourModel::
save() const
{
    /*
     * If save() was overrided, PBNodeDataModel::save() must be called explicitely.
     */
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams[ "contourMode" ] = mParams.miContourMode;
    cParams[ "contourMethod" ] = mParams.miContourMethod;
    cParams[ "bValue" ] = mParams.mucBValue;
    cParams[ "gValue" ] = mParams.mucGValue;
    cParams[ "rValue" ] = mParams.mucRValue;
    cParams[ "lineThickness" ] = mParams.miLineThickness;
    cParams[ "lineType" ] = mParams.miLineType;
    cParams[ "contourCount" ] = mProps.miContourCount;
    modelJson[ "cParams" ] = cParams;

    return modelJson;
}

void
DrawContourModel::
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
        v = paramsObj[ "bValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "b_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();

            mParams.mucBValue = v.toInt();
        }
        v = paramsObj[ "gVlaue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "g_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();

            mParams.mucGValue = v.toInt();
        }
        v = paramsObj[ "rValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "r_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();

            mParams.mucRValue = v.toInt();
        }
        v = paramsObj[ "lineThickness" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_thickness" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < IntPropertyType > >(prop);
            typedProp->getData().miValue = v.toInt();

            mParams.miLineThickness = v.toInt();
        }
        v = paramsObj[ "lineType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < EnumPropertyType > >(prop);
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miLineType = v.toInt();
        }
        v = paramsObj[ "contourCount" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "contour_count" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < QString > >(prop);
            typedProp->getData() = v.toString();

            mProps.miContourCount = v.toInt();
        }
    }


}

void
DrawContourModel::
setModelProperty( QString & id, const QVariant & value )
{
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
    else if(id=="b_value")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        TypedProp->getData().mucValue = value.toInt();
        mParams.mucBValue = value.toInt();
    }
    else if(id=="g_value")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        TypedProp->getData().mucValue = value.toInt();
        mParams.mucGValue = value.toInt();
    }
    else if(id=="r_value")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        TypedProp->getData().mucValue = value.toInt();
        mParams.mucRValue = value.toInt();
    }
    else if(id=="line_thickness")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        TypedProp->getData().miValue = value.toInt();
        mParams.miLineThickness = value.toInt();
    }
    else if(id=="line_type")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<EnumPropertyType>>(prop);
        TypedProp->getData().miCurrentIndex = value.toInt();
        switch(value.toInt())
        {
        case 0:
            mParams.miLineType = cv::LINE_8;
            break;

        case 1:
            mParams.miLineType = cv::LINE_4;
            break;

        case 2:
            mParams.miLineType = cv::LINE_AA;
            break;
        }
    }

    if(mpCVImageInData)
    {
        processData(mpCVImageInData,mpCVImageData,mParams,mProps);

        Q_EMIT dataUpdated(0);
    }
}

void DrawContourModel::processData(const std::shared_ptr<CVImageData> &in, std::shared_ptr<CVImageData> &out, const DrawContourParameters &params, DrawContourProperties &props)
{
    cv::Mat cvTemp;
    cv::Mat& in_image = in->image();
    cv::Mat& out_image = out->image();
    std::vector<std::vector<cv::Point>> vvPtContours;
    std::vector<cv::Vec4i> vV4iHierarchy;
    if(in_image.channels()==1)
    {
        cvTemp = in_image.clone();
        cv::cvtColor(in_image,out_image,cv::COLOR_GRAY2BGR);
    }
    else
    {
        out_image = in_image.clone();
        cv::cvtColor(in_image,cvTemp,cv::COLOR_BGR2GRAY);
    }
    cv::findContours(cvTemp,vvPtContours,vV4iHierarchy,params.miContourMode,params.miContourMethod);
    cv::drawContours(out_image,vvPtContours,-1,cv::Vec3b(static_cast<uchar>(params.mucBValue),static_cast<uchar>(params.mucGValue),static_cast<uchar>(params.mucRValue)),params.miLineThickness,params.miLineType);
    props.miContourCount = static_cast<int>(vvPtContours.size());

    auto prop = mMapIdToProperty["contour_count"];
    auto typedProp = std::static_pointer_cast<TypedProperty<QString>>(prop);
    typedProp->getData() = QString("%1").arg(mProps.miContourCount);
}

const QString DrawContourModel::_category = QString( "Image Processing" );

const QString DrawContourModel::_model_name = QString( "Draw Contour" );
