#include "ColorSpaceModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

ColorSpaceModel::
ColorSpaceModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":ColorSpace.png" )
{
    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList( {"BGR", "RGB", "YCrCb"} );
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "color_space_input";
    auto propColorSpaceInput = std::make_shared< TypedProperty< EnumPropertyType > >( "Input Color Space", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType );
    mvProperty.push_back( propColorSpaceInput );
    mMapIdToProperty[ propId ] = propColorSpaceInput;

    enumPropertyType.mslEnumNames = QStringList( {"BGR", "RGB", "YCrCb"} );
    enumPropertyType.miCurrentIndex = 1;
    propId = "color_space_output";
    auto propColorSpaceOutput = std::make_shared< TypedProperty< EnumPropertyType > >( "Output Color Space", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType );
    mvProperty.push_back( propColorSpaceOutput );
    mMapIdToProperty[ propId ] = propColorSpaceOutput;
}

unsigned int
ColorSpaceModel::
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
ColorSpaceModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
ColorSpaceModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
ColorSpaceModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            cv::Mat cvColorSpaceImage = processData(mParams,d);
            mpCVImageData = std::make_shared<CVImageData>(cvColorSpaceImage);
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
ColorSpaceModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["colorSpaceInput"] = mParams.miColorSpaceInput;
    cParams["colorSpaceOutput"] = mParams.miColorSpaceOutput;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ColorSpaceModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v =  paramsObj[ "colorSpaceInput" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "color_space_input" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miColorSpaceInput = v.toInt();
        }
        v = paramsObj[ "colorSpaceOutput" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "color_space_output" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miColorSpaceOutput = v.toInt();
        }
    }
}

void
ColorSpaceModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "color_space_input" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        mParams.miColorSpaceInput = value.toInt();
    }
    else if( id == "color_space_output" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        mParams.miColorSpaceOutput = value.toInt();
    }
    if( mpCVImageInData )
    {
        cv::Mat cvColorSpaceImage = processData(mParams,mpCVImageInData);
        mpCVImageData = std::make_shared<CVImageData>(cvColorSpaceImage);

        Q_EMIT dataUpdated(0);
    }
}

cv::Mat ColorSpaceModel::processData(const ColorSpaceParameters &mParams, const std::shared_ptr<CVImageData> &p)
{
    cv::Mat Output;
    if(mParams.miColorSpaceInput == mParams.miColorSpaceOutput)
    {
        Output = p->image().clone();
    }
    else
    {
        int cvColorSpaceConvertion;
        switch(mParams.miColorSpaceInput)
        {
        case 0 :
            switch(mParams.miColorSpaceOutput)
            {
            case 1 :
                cvColorSpaceConvertion = cv::COLOR_BGR2RGB;
                break;

            case 2 :
                cvColorSpaceConvertion = cv::COLOR_BGR2YCrCb;
                break;
            }
            break;

        case 1 :
            switch(mParams.miColorSpaceOutput)
            {
            case 0 :
                cvColorSpaceConvertion = cv::COLOR_RGB2BGR;
                break;

            case 2 :
                cvColorSpaceConvertion = cv::COLOR_RGB2YCrCb;
                break;
            }
            break;

        case 2:
            switch(mParams.miColorSpaceOutput)
            {
            case 0 :
                cvColorSpaceConvertion = cv::COLOR_YCrCb2BGR;
                break;

            case 1 :
                cvColorSpaceConvertion = cv::COLOR_YCrCb2RGB;
                break;
            }
            break;
        }
        cv::cvtColor(p->image(),Output,cvColorSpaceConvertion);
    }
    return Output;
}

const QString ColorSpaceModel::_category = QString( "Image Operation" );

const QString ColorSpaceModel::_model_name = QString( "Color Space" );
