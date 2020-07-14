#include "MorphologicalTransformationModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

MorphologicalTransformationModel::
MorphologicalTransformationModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":MorphologicalTransformation.png" )
{
    mpCVImageData = std::make_shared<CVImageData>(cv::Mat());

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"MORPH_OPEN", "MORPH_CLOSE", "MORPH_GRADIENT", "MORPH_TOPHAT", "MORPH_BLACKHAT"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "morph_method";
    auto propMorphMethod = std::make_shared<TypedProperty<EnumPropertyType>>("Iterations", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back(propMorphMethod);
    mMapIdToProperty[propId] = propMorphMethod;

    PointPropertyType pointPropertyType; //need additional type support from the function displaying properties in the UI.
    pointPropertyType.miXPosition = mParams.mCVPointAnchor.x;
    pointPropertyType.miYPosition = mParams.mCVPointAnchor.y;
    propId = "anchor_point";
    auto propAnchorPoint = std::make_shared< TypedProperty< PointPropertyType > >( "Anchor Point", propId, QVariant::Point, pointPropertyType ,"Operation");
    mvProperty.push_back( propAnchorPoint );
    mMapIdToProperty[ propId ] = propAnchorPoint;

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miIteration;
    propId = "iteration";
    auto propIteration = std::make_shared<TypedProperty<IntPropertyType>>("Iterations", propId, QVariant::Int, intPropertyType, "Operation");
    mvProperty.push_back(propIteration);
    mMapIdToProperty[propId] = propIteration;

    enumPropertyType.mslEnumNames = QStringList( {"DEFAULT", "CONSTANT", "REPLICATE", "REFLECT", "WRAP", "TRANSPARENT", "ISOLATED"} );
    enumPropertyType.miCurrentIndex = 0;
    propId = "border_type";
    auto propBorderType = std::make_shared< TypedProperty< EnumPropertyType > >( "Border Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Display" );
    mvProperty.push_back( propBorderType );
    mMapIdToProperty[ propId ] = propBorderType;
}

unsigned int
MorphologicalTransformationModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
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
MorphologicalTransformationModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
MorphologicalTransformationModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
MorphologicalTransformationModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mapCVImageInData[portIndex] = d;
            if(mapCVImageInData[0] && mapCVImageInData[1])
            {
                processData(mapCVImageInData,mpCVImageData,mParams);
            }
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
MorphologicalTransformationModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["morphMethod"] = mParams.miMorphMethod;
    cParams["anchorX"] = mParams.mCVPointAnchor.x;
    cParams["anchorY"] = mParams.mCVPointAnchor.y;
    cParams["iteration"] = mParams.miIteration;
    cParams["borderType"] = mParams.miBorderType;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
MorphologicalTransformationModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "morphMethod" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "morph_method" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();
            mParams.miMorphMethod = v.toInt();
        }

        QJsonValue argX = paramsObj[ "anchorX" ];
        QJsonValue argY = paramsObj[ "anchorY" ];
        if( !argX.isUndefined() && ! argY.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "anchor_point" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = argX.toInt();
            typedProp->getData().miYPosition = argY.toInt();

            mParams.mCVPointAnchor = cv::Point(argX.toInt(),argY.toInt());
        }
        v = paramsObj[ "iteration" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty["iteration"];
            auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
            typedProp->getData().miValue = v.toInt();

            mParams.miIteration = v.toInt();
        }
        v = paramsObj[ "borderType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "border_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();
            mParams.miBorderType = v.toInt();
        }
    }
}

void
MorphologicalTransformationModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "morph_method" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < EnumPropertyType >>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        switch( value.toInt() )
        {
        case 0:
            mParams.miMorphMethod = cv::MORPH_OPEN;
            break;
        case 1:
            mParams.miMorphMethod = cv::MORPH_CLOSE;
            break;
        case 2:
            mParams.miMorphMethod = cv::MORPH_GRADIENT;
            break;
        case 3:
            mParams.miMorphMethod = cv::MORPH_TOPHAT;
            break;
        case 4:
            mParams.miMorphMethod = cv::MORPH_BLACKHAT;
            break;
        }
    }
    else if( id == "anchor_point" && mapCVImageInData[1] && !mapCVImageInData[1]->image().empty())
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        QPoint aPoint =  value.toPoint();
        const int& maxX = mapCVImageInData[1]->image().cols;
        const int& maxY = mapCVImageInData[1]->image().rows;
        bool adjValue = false;
        if( aPoint.x() > maxX) //Size members are gauranteed to be odd numbers.
        {
            aPoint.setX(maxX);
            adjValue = true;
        }
        else if( aPoint.x() < -1)
        {
            aPoint.setX(-1);
            adjValue = true;
        }
        if( aPoint.y() > maxY )
        {
            aPoint.setY(maxY);
            adjValue = true;
        }
        else if( aPoint.y() < -1)
        {
            aPoint.setY(-1);
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miXPosition = aPoint.x();
            typedProp->getData().miYPosition = aPoint.y();

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = aPoint.x();
            typedProp->getData().miYPosition = aPoint.y();

            mParams.mCVPointAnchor = cv::Point( aPoint.x(), aPoint.y() );
        }
    }
    else if( id == "iterations" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < IntPropertyType >>(prop);
        typedProp->getData().miValue = value.toInt();

        mParams.miIteration = value.toInt();
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
            break;
        case 6: // ISOLATED
            mParams.miBorderType = cv::BORDER_ISOLATED;
            break;
        }
    }
    if( mapCVImageInData[0] && mapCVImageInData[1] )
    {
        processData(mapCVImageInData,mpCVImageData,mParams);

        Q_EMIT dataUpdated(0);
    }
}

void MorphologicalTransformationModel::processData(const std::shared_ptr<CVImageData> (&in)[2], std::shared_ptr<CVImageData> &out, const MorphologicalTransformationParameters &params)
{
    cv::Mat& in_image = in[0]->image();
    cv::Mat& in_kernel = in[1]->image();
    if(in_image.empty() || (in_image.depth()!=CV_8U &&
       in_image.depth()!=CV_16U && in_image.depth()!=CV_16S &&
       in_image.depth()!=CV_32F && in_image.depth()!=CV_64F) ||
       in_kernel.empty() || in_kernel.type()!=CV_8UC1 ||
       in_kernel.rows>in_image.rows || in_kernel.cols>in_image.cols)
    {
        return;
    }
    double min;
    double max;
    cv::minMaxLoc(in_kernel,&min,&max);
    if(min==0 && max==1)
    {
        cv::morphologyEx(in_image,
                         out->image(),
                         params.miMorphMethod,
                         in_kernel,
                         params.mCVPointAnchor,
                         params.miIteration,
                         params.miBorderType);
    }
}

const QString MorphologicalTransformationModel::_category = QString( "Image Modification" );

const QString MorphologicalTransformationModel::_model_name = QString( "Morph Transformation" );
