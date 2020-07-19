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
    mpIntegerData = std::make_shared< IntegerData >( int() );
    mpSyncData = std::make_shared< SyncData >();

    IntPropertyType intPropertyType;
    intPropertyType.miMin = -1;
    intPropertyType.miValue = mParams.miContourIndex;
    QString propId = "contour_index";
    auto propContourIndex = std::make_shared<TypedProperty<IntPropertyType>>("Contour Index",propId,QVariant::Int,intPropertyType, "Display");
    mvProperty.push_back(propContourIndex);
    mMapIdToProperty[propId] = propContourIndex;

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

    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared<TypedProperty<IntPropertyType>>("Line Thickness",propId,QVariant::Int,intPropertyType, "Display");
    mvProperty.push_back(propLineThickness);
    mMapIdToProperty[propId] = propLineThickness;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "line_type";
    auto propLineType = std::make_shared<TypedProperty<EnumPropertyType>>("Line Type",propId,QtVariantPropertyManager::enumTypeId(),enumPropertyType, "Display");
    mvProperty.push_back(propLineType);
    mMapIdToProperty[propId] = propLineType;
}

unsigned int
DrawContourModel::
nPorts( PortType portType ) const
{
    switch( portType )
    {
    case PortType::In:
        return( 4 );
    case PortType::Out:
        return( 3 );
    default:
        return( -1 );
    }
}

NodeDataType
DrawContourModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        return CVImageData().type();
    }
    else if(portIndex == 1)
    {
        if(portType == PortType::In)
        {
            return StdVectorData<std::vector<cv::Point>>().type();
        }
        else if(portType == PortType::Out)
        {
            return IntegerData().type();
        }
    }
    else if(portIndex == 2)
    {
        if(portType == PortType::In)
        {
            return StdVectorData<cv::Vec4i>().type();
        }
        else if(portType == PortType::Out)
        {
            return SyncData().type();
        }
    }
    else if(portIndex == 3)
    {
        return IntegerData().type();
    }
    return NodeDataType();
}

std::shared_ptr<NodeData>
DrawContourModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I == 0)
        {
            return mpCVImageData;
        }
        else if(I == 1)
        {
            return mpIntegerData;
        }
        else if(I == 2)
        {
            return mpSyncData;
        }
    }
    return nullptr;
}

void
DrawContourModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex portIndex)
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(2);
        if(portIndex == 0)
        {
            auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
            if( d )
            {
                mpCVImageInData = d;
            }
        }
        else if(portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast<StdVectorData<std::vector<cv::Point>>>(nodeData);
            if(d)
            {
                mpStdVectorInData_StdVector_CVPoint = d;
                overwrite(mpStdVectorInData_StdVector_CVPoint,mParams);
            }
        }
        else if(portIndex == 2)
        {
            auto d = std::dynamic_pointer_cast<StdVectorData<cv::Vec4i>>(nodeData);
            if(d)
            {
                mpStdVectorInData_CVVec4i = d;
            }
        }
        else if(portIndex == 3)
        {
            auto d = std::dynamic_pointer_cast<IntegerData>(nodeData);
            if(d)
            {
                mpIntegerInData = d;
                overwrite(mpIntegerInData,mParams);
            }
        }
        if(mpCVImageInData && mpStdVectorInData_StdVector_CVPoint)
        {
            processData(mpCVImageInData,
                        mpStdVectorInData_StdVector_CVPoint,
                        mpStdVectorInData_CVVec4i,
                        mpCVImageData,
                        mpIntegerData,
                        mParams);
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(2);
    }
    Q_EMIT dataUpdated(0);
    Q_EMIT dataUpdated(1);
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
    cParams[ "contourIndex" ] = mParams.miContourIndex;
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
        QJsonValue v = paramsObj[ "contourIndex" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "contour_index" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >(prop);
            typedProp->getData().miValue = v.toInt();

            mParams.miContourIndex = v.toInt();
        }
        v = paramsObj[ "bValue" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "b_value" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >(prop);
            typedProp->getData().mucValue = v.toInt();

            mParams.mucBValue = v.toInt();
        }
        v = paramsObj[ "gValue" ];
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
    }


}

void
DrawContourModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(2);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if(id=="contour_index")
    {
        auto TypedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        TypedProp->getData().miValue = value.toInt();
        mParams.miContourIndex = value.toInt();
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

    if(mpCVImageInData && mpStdVectorInData_StdVector_CVPoint)
    {
        processData(mpCVImageInData,
                    mpStdVectorInData_StdVector_CVPoint,
                    mpStdVectorInData_CVVec4i,
                    mpCVImageData,
                    mpIntegerData,
                    mParams);
        Q_EMIT dataUpdated(0);
        Q_EMIT dataUpdated(1);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(2);
}

void DrawContourModel::
processData(const std::shared_ptr<CVImageData> &inImage,
                 const std::shared_ptr<StdVectorData<std::vector<cv::Point>>>& inVecVec,
                 const std::shared_ptr<StdVectorData<cv::Vec4i>>& inVec,
                 std::shared_ptr<CVImageData> &outImage,
                 std::shared_ptr<IntegerData> &outInt,
                 const DrawContourParameters& params)
{
    cv::Mat& in_image = inImage->image();
    if(in_image.empty() || in_image.type()!=CV_8UC3)
    {
        return;
    }
    cv::Mat& out_image = outImage->image();
    outImage->set_image(in_image);
    auto& in_contour = inVecVec->vector();
    //A simple validation algorithm. Time complexity needs to be decreased.
    for(const std::vector<cv::Point>& vPoint : in_contour)
    {
        for(const cv::Point& point : vPoint)
        {
            if(point.x > out_image.cols || point.x < 0 || point.y > out_image.rows || point.y < 0)
            {
                return;
            }
        }
    }
    //Hierarchy cannot be validated at the moment.
    if(inVec!=nullptr)
    {
        cv::drawContours(out_image,
                         in_contour,
                         params.miContourIndex,
                         cv::Vec3b(static_cast<uchar>(params.mucBValue),
                                   static_cast<uchar>(params.mucGValue),
                                   static_cast<uchar>(params.mucRValue)),
                         params.miLineThickness,
                         params.miLineType,
                         inVec->vector(),
                         INT_MAX);
    }
    else
    {
        cv::drawContours(out_image,
                         in_contour,
                         params.miContourIndex,
                         cv::Vec3b(static_cast<uchar>(params.mucBValue),
                                   static_cast<uchar>(params.mucGValue),
                                   static_cast<uchar>(params.mucRValue)),
                         params.miLineThickness,
                         params.miLineType);
    }
    outInt->number() = static_cast<int>(in_contour.size());
}

void
DrawContourModel::
overwrite(const std::shared_ptr<StdVectorData<std::vector<cv::Point>>> &in, DrawContourParameters &params)
{
    const int size = static_cast<int>(in->vector().size());
    auto prop = mMapIdToProperty["contour_index"];
    auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
    typedProp->getData().miValue = typedProp->getData().miMin;
    typedProp->getData().miMax = size-1;
    params.miContourIndex = -1;
}

void
DrawContourModel::
overwrite(const std::shared_ptr<IntegerData> &in, DrawContourParameters &params)
{
    const int& in_number = in->number();
    if(in_number>=-1 && in_number<static_cast<int>(mpStdVectorInData_StdVector_CVPoint->vector().size()))
    {
        auto prop = mMapIdToProperty["contour_index"];
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miValue = in_number;
        params.miContourIndex = in_number;
    }
}

const QString DrawContourModel::_category = QString( "Image Analysis" );

const QString DrawContourModel::_model_name = QString( "Draw Contour" );
