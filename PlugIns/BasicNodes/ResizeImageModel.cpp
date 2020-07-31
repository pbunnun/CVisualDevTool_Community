#include "ResizeImageModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

ResizeImageModel::
ResizeImageModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":ResizeImage.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpSyncData = std::make_shared< SyncData >();

    SizePropertyType sizePropertyType;
    sizePropertyType.miWidth = mParams.mCVSizeOutput.width;
    sizePropertyType.miHeight = mParams.mCVSizeOutput.height;
    QString propId = "output_size";
    auto propOutputSize = std::make_shared<TypedProperty<SizePropertyType>>("Output Size", propId, QVariant::Size, sizePropertyType, "Operation");
    mvProperty.push_back(propOutputSize);
    mMapIdToProperty[ propId ] = propOutputSize;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdFactorX;
    propId = "factor_x";
    auto propFactorX = std::make_shared<TypedProperty<DoublePropertyType>>("fx", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back(propFactorX);
    mMapIdToProperty[ propId ] = propFactorX;

    doublePropertyType.mdValue = mParams.mdFactorX;
    propId = "factor_y";
    auto propFactorY = std::make_shared<TypedProperty<DoublePropertyType>>("fy", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back(propFactorY);
    mMapIdToProperty[ propId ] = propFactorY;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"INTER_NEAREST", "INTER_LINEAR", "INTER_AREA", "INTER_CUBIC", "INTER_LANCZOS4"});
    enumPropertyType.miCurrentIndex = 1;
    propId = "interpolation";
    auto propInterpolation = std::make_shared< TypedProperty< EnumPropertyType > >( "Interpolation", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propInterpolation );
    mMapIdToProperty[ propId ] = propInterpolation;
}

unsigned int
ResizeImageModel::
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
ResizeImageModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        return CVImageData().type();
    }
    else if( portIndex == 1 )
    {
        if(portType == PortType::In)
            return CVSizeData().type();
        else if(portType == PortType::Out)
            return SyncData().type();
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
ResizeImageModel::
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
ResizeImageModel::
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
            }
        }
        else if(portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast<CVSizeData>(nodeData);
            if (d)
            {
                mpCVSizeInData = d;
                overwrite(mpCVSizeInData,mParams);
            }
        }
        if(mpCVImageInData)
        {
            processData( mpCVImageInData, mpCVImageData, mParams );
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
ResizeImageModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["outputSizeWidth"] = mParams.mCVSizeOutput.width;
    cParams["outputSizeHeight"] = mParams.mCVSizeOutput.height;
    cParams["factorX"] = mParams.mdFactorX;
    cParams["factorY"] = mParams.mdFactorY;
    cParams["interpolation"] = mParams.miInterpolation;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ResizeImageModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "outputSizeWidth" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "output_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
            typedProp->getData().miWidth = v.toInt();
            mParams.mCVSizeOutput.width = v.toInt();
        }
        v = paramsObj[ "outputSizeHeight" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "output_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
            typedProp->getData().miHeight = v.toInt();
            mParams.mCVSizeOutput.height = v.toInt();
        }
        v =  paramsObj[ "factorX" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "factor_x" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdFactorX = v.toDouble();
        }
        v =  paramsObj[ "factorY" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "factor_y" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdFactorY = v.toDouble();
        }
        v =  paramsObj[ "interpolation" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "interpolation" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miInterpolation = v.toInt();
        }
    }
}

void
ResizeImageModel::
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
        QSize oSize =  value.toSize();
        bool adjValue = false;
        if( (oSize.width()==0 || oSize.height()==0) &&
            (mParams.mdFactorX == 0 || mParams.mdFactorY == 0) )
        {
            oSize = QSize(300,300);
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miWidth = oSize.width();
            typedProp->getData().miHeight = oSize.height();

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< SizePropertyType > >( prop );
            typedProp->getData().miWidth = oSize.width();
            typedProp->getData().miHeight = oSize.height();

            mParams.mCVSizeOutput = cv::Size(oSize.width(),oSize.height());
        }
    }
    else if( id == "factor_x" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        double fX = value.toDouble();
        bool adjValue = false;
        if( (mParams.mCVSizeOutput.width==0 ||
             mParams.mCVSizeOutput.height==0) && fX==0 )
        {
            fX = 1;
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().mdValue = fX;

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = fX;

            mParams.mdFactorX = fX;
        }
    }
    else if( id == "factor_y" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        double fY = value.toDouble();
        bool adjValue = false;
        if( (mParams.mCVSizeOutput.width==0 ||
             mParams.mCVSizeOutput.height==0) && fY==0 )
        {
            fY = 1;
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().mdValue = fY;

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = fY;

            mParams.mdFactorY = fY;
        }
    }
    else if( id == "interpolation" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miInterpolation = cv::INTER_NEAREST;
            break;

        case 1:
            mParams.miInterpolation = cv::INTER_LINEAR;
            break;

        case 2:
            mParams.miInterpolation = cv::INTER_AREA;
            break;

        case 3:
            mParams.miInterpolation = cv::INTER_CUBIC;
            break;

        case 4:
            mParams.miInterpolation = cv::INTER_LANCZOS4;
            break;
        }
    }

    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mParams );

        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
ResizeImageModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> & out,
            const ResizeImageParameters & params )
{
    cv::Mat& in_image = in->image();
   if(!in_image.empty())
   {
       cv::resize(in_image,
                  out->image(),
                  params.mCVSizeOutput,
                  params.mdFactorX,
                  params.mdFactorY,
                  params.miInterpolation);
   }
}

void
ResizeImageModel::
overwrite(const std::shared_ptr<CVSizeData> &in, ResizeImageParameters &params)
{
    const cv::Size& in_size = in->size();
    if( (in_size.width!=0 && in_size.height!=0) ||
        (params.mdFactorX!=0 && params.mdFactorY!=0) )
    {
        auto prop = mMapIdToProperty["output_size"];
        auto typedProp = std::static_pointer_cast<TypedProperty<SizePropertyType>>(prop);
        typedProp->getData().miWidth = in_size.width;
        typedProp->getData().miHeight = in_size.height;

        params.mCVSizeOutput = in_size;
    }
}

const QString ResizeImageModel::_category = QString( "Image Conversion" );

const QString ResizeImageModel::_model_name = QString( "Resize Image" );
