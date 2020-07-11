#include "DrawCollectionElementsModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "qtvariantproperty.h"

DrawCollectionElementsModel::
DrawCollectionElementsModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":DrawCollectionElements.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpSyncData = std::make_shared< SyncData >();

    QString propId = "display_lines";
    auto propDisplayLines = std::make_shared<TypedProperty<bool>>("Display Lines", propId, QVariant::Bool, mParams.mbDisplayLines, "Operation");
    mvProperty.push_back(propDisplayLines);
    mMapIdToProperty[ propId ] = propDisplayLines;

    UcharPropertyType ucharPropertyType;
    for(int i=0; i<3; i++)
    {
        ucharPropertyType.mucValue = mParams.mucLineColor[i];
        propId = QString("line_color_%1").arg(i);
        QString lineColor = QString::fromStdString("Line Color "+color[i]);
        auto propLineColor = std::make_shared<TypedProperty<UcharPropertyType>>(lineColor, propId, QVariant::Int, ucharPropertyType, "Operation");
        mvProperty.push_back(propLineColor);
        mMapIdToProperty[ propId ] = propLineColor;
    }

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared<TypedProperty<IntPropertyType>>("Line Thickness", propId, QVariant::Int, intPropertyType, "Operation");
    mvProperty.push_back( propLineThickness );
    mMapIdToProperty[ propId ] = propLineThickness;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 2;
    propId = "line_type";
    auto propLineType = std::make_shared<TypedProperty<EnumPropertyType>>("Line Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propLineType );
    mMapIdToProperty[ propId ] = propLineType;
}

unsigned int
DrawCollectionElementsModel::
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
DrawCollectionElementsModel::
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
            return StdVectorData<cv::Point>().type();
        }
        else if(portType == PortType::Out)
        {
            return SyncData().type();
        }
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
DrawCollectionElementsModel::
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
            return mpSyncData;
        }
    }
    return nullptr;
}

void
DrawCollectionElementsModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        if(portIndex == 0)
        {
            auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
            if (d)
            {
                mpCVImageInData = d;
                if(mpCVImageInData && mpStdVectorInData_CVPoint)
                {
                    processData( mpCVImageInData, mpStdVectorInData_CVPoint, mpCVImageData, mParams);
                }
            }
        }
        else if(portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast<StdVectorData<cv::Point>>(nodeData);
            if (d)
            {
                mpStdVectorInData_CVPoint = d;
                if(mpCVImageInData && mpStdVectorInData_CVPoint)
                {
                    processData( mpCVImageInData, mpStdVectorInData_CVPoint, mpCVImageData, mParams);
                }
            }
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
DrawCollectionElementsModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["displayLines"] = mParams.mbDisplayLines;
    for(int i=0; i<3; i++)
    {
        cParams[QString("lineColor%1").arg(i)] = mParams.mucLineColor[i];
    }
    cParams["lineThickness"] = mParams.miLineThickness;
    cParams["lineType"] = mParams.miLineType;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
DrawCollectionElementsModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "displayLines" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "display_lines" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < bool > > (prop);
            typedProp->getData() = v.toBool();

            mParams.mbDisplayLines = v.toBool();
        }
        for(int i=0; i<3; i++)
        {
            v = paramsObj[QString("lineColor%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("line_color_%1").arg(i)];
                auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
                typedProp->getData().mucValue = v.toInt();

                mParams.mucLineColor[i] = v.toInt();
            }
        }
        v = paramsObj[ "lineThickness" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_thickness" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
            typedProp->getData().miValue = v.toInt();

            mParams.miLineThickness = v.toInt();
        }
        v = paramsObj[ "lineType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_type" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<EnumPropertyType>>(prop);
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miLineType = v.toInt();
        }
    }
}

void
DrawCollectionElementsModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "display_lines" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbDisplayLines = value.toBool();
    }
    for(int i=0; i<3; i++)
    {
        if(id==QString("line_color_%1").arg(i))
        {
            auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = value.toInt();

            mParams.mucLineColor[i] = value.toInt();
        }
    }
    if( id == "line_thickness" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty <IntPropertyType>>(prop);
        typedProp->getData().miValue = value.toInt();

        mParams.miLineThickness = value.toInt();
    }
    else if( id == "line_type" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty <EnumPropertyType>>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

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

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpStdVectorInData_CVPoint, mpCVImageData, mParams);

        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
DrawCollectionElementsModel::
processData(const std::shared_ptr< CVImageData > & inImage, std::shared_ptr<StdVectorData<cv::Point>> & inVec,
            std::shared_ptr<CVImageData> &out, const DrawCollectionElementsParameters & params)
{
    if(params.mbDisplayLines)
    {
        cv::Mat& in_image = inImage->image();
        if(in_image.empty() || in_image.type()!=CV_8UC3)
        {
            return;
        }
        out->set_image(in_image);
        const cv::Scalar colors(params.mucLineColor[0],
                                params.mucLineColor[1],
                                params.mucLineColor[2]);
        for(const cv::Point& point : inVec->vector())
        {
            cv::circle(out->image(),
                       point,
                       1,
                       colors,
                       params.miLineThickness,
                       params.miLineType);
        }
    }
}

const std::string DrawCollectionElementsModel::color[3] = {"B", "G", "R"};

const QString DrawCollectionElementsModel::_category = QString( "Image Analysis" );

const QString DrawCollectionElementsModel::_model_name = QString( "Draw Collection Elements" );
