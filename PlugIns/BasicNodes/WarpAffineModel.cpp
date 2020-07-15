#include "WarpAffineModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

WarpAffineModel::
WarpAffineModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":WarpAffine.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpSyncData = std::make_shared< SyncData >();

    SizePropertyType sizePropertyType;
    sizePropertyType.miWidth = mParams.mCVSizeOutput.width;
    sizePropertyType.miHeight = mParams.mCVSizeOutput.height;
    QString propId = "output_size";
    auto propOutputSize = std::make_shared<TypedProperty<SizePropertyType>>("Output Size", propId, QVariant::Size, sizePropertyType, "Operation");
    mvProperty.push_back( propOutputSize );
    mMapIdToProperty[ propId ] = propOutputSize;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"INTER_NEAREST", "INTER_LINEAR", "INTER_AREA", "INTER_CUBIC", "INTER_LANCZOS4"});
    enumPropertyType.miCurrentIndex = 1;
    propId = "flags";
    auto propFlags = std::make_shared< TypedProperty< EnumPropertyType > >( "Interpolation", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propFlags );
    mMapIdToProperty[ propId ] = propFlags;

    enumPropertyType.mslEnumNames = QStringList( {"DEFAULT", "CONSTANT", "REPLICATE", "REFLECT", "WRAP"} );
    enumPropertyType.miCurrentIndex = 1;
    propId = "border_type";
    auto propBorderType = std::make_shared< TypedProperty< EnumPropertyType > >( "Border Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Display" );
    mvProperty.push_back( propBorderType );
    mMapIdToProperty[ propId ] = propBorderType;

    UcharPropertyType ucharPropertyType;
    for(int i=0; i<3; i++)
    {
        ucharPropertyType.mucValue = mParams.mucBorderColor[i];
        propId = QString("border_color_%1").arg(i);
        QString lineColor = QString::fromStdString("Border Color "+color[i]);
        auto propBoderColor = std::make_shared<TypedProperty<UcharPropertyType>>(lineColor, propId, QVariant::Int, ucharPropertyType, "Operation");
        mvProperty.push_back(propBoderColor);
        mMapIdToProperty[ propId ] = propBoderColor;
    }
}

unsigned int
WarpAffineModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 3;
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
WarpAffineModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if( portType==PortType::Out && portIndex == 1 )
    {
        return SyncData().type();
    }
    else if(portType == PortType::In && portIndex == 2)
    {
        return CVSizeData().type();
    }
    return CVImageData().type();
}


std::shared_ptr<NodeData>
WarpAffineModel::
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
WarpAffineModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        if (portIndex == 0 || portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
            if(d)
            {
                mapCVImageInData[portIndex] = d;
            }
        }
        else if(portIndex == 2)
        {
            auto d = std::dynamic_pointer_cast<CVSizeData>(nodeData);
            if(d)
            {
                mpCVSizeInData = d;
            }
        }
        if(mpCVSizeInData)
        {
            overwrite(mpCVSizeInData,mParams);
            mpCVSizeInData.reset();
        }
        if(mapCVImageInData[0] && mapCVImageInData[1])
        {
            processData( mapCVImageInData, mpCVImageData, mParams );
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
WarpAffineModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["outputSizeWidth"] = mParams.mCVSizeOutput.width;
    cParams["outputSizeHeight"] = mParams.mCVSizeOutput.height;
    cParams["flags"] = mParams.miFlags;
    cParams["borderType"] = mParams.miBorderType;
    for(int i=0; i<3; i++)
    {
        cParams[QString("borderColor%1").arg(i)] = mParams.mucBorderColor[i];
    }
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
WarpAffineModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue argX = paramsObj[ "outputSizeWidth" ];
        QJsonValue argY = paramsObj[ "outputSizeHeight" ];
        if( !argX.isUndefined() && !argY.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "output_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
            typedProp->getData().miWidth = argX.toInt();
            typedProp->getData().miHeight =argY.toInt();

            mParams.mCVSizeOutput = cv::Size(argX.toInt(), argY.toInt());
        }
        QJsonValue v =  paramsObj[ "flags" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "flags" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miFlags = v.toInt();
        }
        v =  paramsObj[ "borderType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "border_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miBorderType = v.toInt();
        }
        for(int i=0; i<3; i++)
        {
            v = paramsObj[QString("borderColor%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("border_color_%1").arg(i)];
                auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
                typedProp->getData().mucValue = v.toInt();

                mParams.mucBorderColor[i] = v.toInt();
            }
        }
    }
}

void
WarpAffineModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "output_size" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
        const QSize oSize = value.toSize();
        typedProp->getData().miWidth = oSize.width();
        typedProp->getData().miHeight = oSize.height();

        mParams.mCVSizeOutput = cv::Size(oSize.width(),oSize.height());
    }
    else if( id == "flags" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miFlags = cv::INTER_NEAREST;
            break;

        case 1:
            mParams.miFlags = cv::INTER_LINEAR;
            break;

        case 2:
            mParams.miFlags = cv::INTER_AREA;
            break;

        case 3:
            mParams.miFlags = cv::INTER_CUBIC;
            break;

        case 4:
            mParams.miFlags = cv::INTER_LANCZOS4;
            break;
        }
    }
    else if( id == "border_type" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch( value.toInt() )
        {
        case 0: // DEFAULT
            mParams.miBorderType = cv::BORDER_DEFAULT;
            break;
        case 1: // CONSTANT
            mParams.miBorderType = cv::BORDER_CONSTANT;
            break;
        case 2: // REPLICATE
            mParams.miBorderType = cv::BORDER_REPLICATE;
            break;
        case 3: // REFLECT
            mParams.miBorderType = cv::BORDER_REFLECT;
            break;
        case 4: // WRAP
            mParams.miBorderType = cv::BORDER_WRAP;
            break;
        case 5: // TRANSPARENT
            mParams.miBorderType = cv::BORDER_TRANSPARENT;
            break; //Bug occured when this case is executed
        case 6: // ISOLATED
            mParams.miBorderType = cv::BORDER_ISOLATED;
            break;
        }
    }
    for(int i=0; i<3; i++)
    {
        if(id==QString("border_color_%1").arg(i))
        {
            auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = value.toInt();

            mParams.mucBorderColor[i] = value.toInt();
        }
    }

    if( mapCVImageInData[0] && mapCVImageInData[1] )
    {
        processData( mapCVImageInData, mpCVImageData, mParams );

        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
WarpAffineModel::
processData(const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr<CVImageData> & out,
            const WarpAffineParameters & params )
{
    cv::Mat& in_image = in[0]->image();
    cv::Mat& matrix = in[1]->image();
    if(in_image.empty() || matrix.empty() || matrix.type()!= CV_64FC1 ||
       matrix.rows!=2 || matrix.cols!=3)
    {
        return;
    }
    cv::warpAffine(in_image,
                   out->image(),
                   matrix,
                   params.mCVSizeOutput,
                   params.miFlags,
                   params.miBorderType,
                   cv::Scalar(static_cast<uchar>(params.mucBorderColor[0]),
                              static_cast<uchar>(params.mucBorderColor[1]),
                              static_cast<uchar>(params.mucBorderColor[2])));
}

void
WarpAffineModel::
overwrite(const std::shared_ptr<CVSizeData> &in, WarpAffineParameters &params)
{
    cv::Size& in_size = in->size();
    auto prop = mMapIdToProperty["output_size"];
    auto typedProp = std::static_pointer_cast<TypedProperty<SizePropertyType>>(prop);
    typedProp->getData().miWidth = in_size.width;
    typedProp->getData().miHeight = in_size.height;

    params.mCVSizeOutput = in_size;
}

const std::string WarpAffineModel::color[3] = {"B","G","R"};

const QString WarpAffineModel::_category = QString( "Image Transformation" );

const QString WarpAffineModel::_model_name = QString( "Warp Affine" );
