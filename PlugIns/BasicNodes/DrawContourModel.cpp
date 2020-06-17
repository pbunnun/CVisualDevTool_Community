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
    auto propContourMode = std::make_shared< TypedProperty< EnumPropertyType > >("Contour Mode", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propContourMode );
    mMapIdToProperty[ propId ] = propContourMode;

    enumPropertyType.mslEnumNames = QStringList( {"CHAIN_APPROX_NONE", "CHAIN_APPROX_SIMPLE", "CHAIN_APPROX_TC89_L1","CHAIN_APPROX_TC89_KCOS"} );
    enumPropertyType.miCurrentIndex = 1; //initializing this to 0 somehow produces an unexpected output
    propId = "contour_method";
    auto propContourMethod = std::make_shared< TypedProperty< EnumPropertyType > >( "Contour Method", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType );
    mvProperty.push_back( propContourMethod );
    mMapIdToProperty[ propId ] = propContourMethod;

    UcharPropertyType ucharPropertyType;
    ucharPropertyType.mucValue = mParams.mucBValue;
    propId = "b_value";
    auto propBValue = std::make_shared<TypedProperty<UcharPropertyType>>("B value",propId,QVariant::Int,ucharPropertyType);
    mvProperty.push_back(propBValue);
    mMapIdToProperty[propId] = propBValue;

    ucharPropertyType.mucValue = mParams.mucGValue;
    propId = "g_value";
    auto propGValue = std::make_shared<TypedProperty<UcharPropertyType>>("G value",propId,QVariant::Int,ucharPropertyType);
    mvProperty.push_back(propGValue);
    mMapIdToProperty[propId] = propGValue;

    ucharPropertyType.mucValue = mParams.mucRValue;
    propId = "r_value";
    auto propRValue = std::make_shared<TypedProperty<UcharPropertyType>>("R value",propId,QVariant::Int,ucharPropertyType);
    mvProperty.push_back(propRValue);
    mMapIdToProperty[propId] = propRValue;

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared<TypedProperty<IntPropertyType>>("Line Thickness",propId,QVariant::Int,intPropertyType);
    mvProperty.push_back(propLineThickness);
    mMapIdToProperty[propId] = propLineThickness;

    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "line_type";
    auto propLineType = std::make_shared<TypedProperty<EnumPropertyType>>("Line Type",propId,QtVariantPropertyManager::enumTypeId(),enumPropertyType);
    mvProperty.push_back(propLineType);
    mMapIdToProperty[propId] = propLineType;

    intPropertyType.miValue = mParams.miContourCount;
    intPropertyType.miMax = mParams.miContourCount;
    intPropertyType.miMin = mParams.miContourCount;
    propId = "contour_count";
    auto propContourCount = std::make_shared<TypedProperty<IntPropertyType>>("Contour Count",propId,QVariant::Int,intPropertyType);
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
    if( isEnable() && mpCVImageData->image().data != nullptr )
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
            cv::Mat cvContourImage = d->image().clone();
            cv::Mat cvTemp;
            std::vector<std::vector<cv::Point>> vvPtContours;
            std::vector<cv::Vec4i> vV4iHierarchy;
            if(d->image().channels()==1)
            {
                cvTemp = d->image().clone();
                cv::cvtColor(cvContourImage,cvContourImage,cv::COLOR_GRAY2BGR);
            }
            else
            {
                cv::cvtColor(d->image(),cvTemp,cv::COLOR_BGR2GRAY);
            }
            cv::findContours(cvTemp,vvPtContours,vV4iHierarchy,mParams.miContourMode,mParams.miContourMethod);
            cv::drawContours(cvContourImage,vvPtContours,-1,cv::Vec3b(static_cast<uchar>(mParams.mucBValue),static_cast<uchar>(mParams.mucGValue),static_cast<uchar>(mParams.mucRValue)),mParams.miLineThickness,mParams.miLineType);
            mpCVImageData = std::make_shared<CVImageData>(cvContourImage);
            auto prop = mMapIdToProperty["contour_count"];
            auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
            typedProp->getData().miMax = static_cast<int>(vvPtContours.size());
            typedProp->getData().miMin = static_cast<int>(vvPtContours.size());
            mParams.miContourCount = static_cast<int>(vvPtContours.size());
        }
    }
    else
    {
        auto prop = mMapIdToProperty["contour_count"];
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miMax = 0;
        typedProp->getData().miMin = 0;
        mParams.miContourCount = 0;
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

        }
        v = paramsObj[ "contourMethod" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "contour_method" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

        }
        v = paramsObj[ "bValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "b_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();
        }
        v = paramsObj[ "gVlaue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "g_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();
        }
        v = paramsObj[ "rValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "r_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();
        }
        v = paramsObj[ "lineThickness" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_thickness" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < IntPropertyType > >(prop);
            typedProp->getData().miValue = v.toInt();
        }
        v = paramsObj[ "lineType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < IntPropertyType > >(prop);
            typedProp->getData().miValue = v.toInt();
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
            mParams.miLineType = cv::LINE_8;
            break;

        case 2:
            mParams.miLineType = cv::LINE_AA;
            break;
        }
    }
    else if(id=="contour count")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miMax = mParams.miContourCount;
        typedProp->getData().miMin = mParams.miContourCount;
    }
    if(mpCVImageInData)
    {
        cv::Mat cvContourImage = mpCVImageInData->image().clone();
        cv::Mat cvTemp;
        std::vector<std::vector<cv::Point>> vvPtContours;
        std::vector<cv::Vec4i> vV4iHierarchy;
        if(mpCVImageInData->image().channels()==1)
        {
            cvTemp = mpCVImageInData->image().clone();
            cv::cvtColor(cvContourImage,cvContourImage,cv::COLOR_GRAY2BGR);
        }
        else
        {
            cv::cvtColor(mpCVImageInData->image(),cvTemp,cv::COLOR_BGR2GRAY);
        }
        cv::findContours(cvTemp,vvPtContours,vV4iHierarchy,mParams.miContourMode,mParams.miContourMethod);
        cv::drawContours(cvContourImage,vvPtContours,-1,cv::Vec3b(static_cast<uchar>(mParams.mucBValue),static_cast<uchar>(mParams.mucGValue),static_cast<uchar>(mParams.mucRValue)),mParams.miLineThickness,mParams.miLineType);
        mpCVImageData = std::make_shared<CVImageData>(cvContourImage);
        auto prop = mMapIdToProperty["contour_count"];
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miMax = static_cast<int>(vvPtContours.size());
        typedProp->getData().miMin = static_cast<int>(vvPtContours.size());
        mParams.miContourCount = static_cast<int>(vvPtContours.size());

        Q_EMIT dataUpdated(0);
    }
}

const QString DrawContourModel::_category = QString( "Image Operation" );

const QString DrawContourModel::_model_name = QString( "Draw Contour" );
