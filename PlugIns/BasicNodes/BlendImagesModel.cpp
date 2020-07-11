#include "BlendImagesModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

BlendImagesModel::
BlendImagesModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget(new BlendImagesEmbeddedWidget),
      _minPixmap( ":BlendImages.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &BlendImagesEmbeddedWidget::radioButton_clicked_signal, this, &BlendImagesModel::em_radioButton_clicked );

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdAlpha;
    doublePropertyType.mdMax = 1.0;
    QString propId = "alpha";
    auto propAlpha = std::make_shared< TypedProperty< DoublePropertyType > >( "Alpha", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propAlpha );
    mMapIdToProperty[ propId ] = propAlpha;

    doublePropertyType.mdValue = mParams.mdBeta;
    doublePropertyType.mdMax = 1.0;
    propId = "beta";
    auto propBeta = std::make_shared< TypedProperty< DoublePropertyType > >( "Beta", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propBeta );
    mMapIdToProperty[ propId ] = propBeta;

    doublePropertyType.mdValue = mParams.mdGamma;
    doublePropertyType.mdMax = 100;
    propId = "gamma";
    auto propGamma = std::make_shared< TypedProperty< DoublePropertyType > >( "Gamma", propId, QVariant::Double, doublePropertyType, "Operation");
    mvProperty.push_back( propGamma );
    mMapIdToProperty[ propId ] = propGamma;

    IntPropertyType intPropertyType;
    mpEmbeddedWidget->setCurrentState(1);
    intPropertyType.miValue = mpEmbeddedWidget->getCurrentState();
    propId = "operation";
    auto propOperation = std::make_shared<TypedProperty<IntPropertyType>>("", propId, QVariant::Int, intPropertyType);
    mMapIdToProperty[ propId ] = propOperation;
}

unsigned int
BlendImagesModel::
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
BlendImagesModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
BlendImagesModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
BlendImagesModel::
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
                processData(mapCVImageInData, mpCVImageData, mParams);
            }
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
BlendImagesModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["alpha"] = mParams.mdAlpha;
    cParams["beta"] = mParams.mdBeta;
    cParams["gamma"] = mParams.mdGamma;
    cParams["operation"] = mpEmbeddedWidget->getCurrentState();
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
BlendImagesModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "alpha" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "alpha" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdAlpha = v.toDouble();
        }
        v = paramsObj[ "beta" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "beta" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdBeta = v.toDouble();
        }
        v = paramsObj[ "gamma" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "gamma" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdGamma = v.toDouble();
        }
        v = paramsObj["operation"];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "operation" ];
            auto typedProp = std::static_pointer_cast<TypedProperty <IntPropertyType> >(prop);
            typedProp->getData().miValue = v.toInt();
            mpEmbeddedWidget->setCurrentState(v.toInt());
        }
    }
}

void
BlendImagesModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "alpha" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdAlpha = value.toDouble();
    }
    else if( id == "beta" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdBeta = value.toDouble();
    }
    else if( id == "gamma" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdGamma = value.toDouble();
    }

    if(mapCVImageInData[0] && mapCVImageInData[1])
    {
        processData( mapCVImageInData, mpCVImageData, mParams );

        Q_EMIT dataUpdated(0);
    }
}

void BlendImagesModel::em_radioButton_clicked()
{
    if(mapCVImageInData[0] && mapCVImageInData[1])
    {
        processData(mapCVImageInData,mpCVImageData,mParams);
        Q_EMIT dataUpdated(0);
    }
}

void
BlendImagesModel::
processData(const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr<CVImageData> & out,
            const BlendImagesParameters & params)
{
    cv::Mat& i0 = in[0]->image();
    cv::Mat& i1 = in[1]->image();
    if(i0.empty() || i1.empty() || i0.type()!= i1.type() || i0.rows!=i1.rows || i0.cols!=i1.cols)
    {
        return;
    }
    switch(mpEmbeddedWidget->getCurrentState())
    {
    case 0:
        cv::add(i0,i1,out->image());
        break;

    case 1:
        cv::addWeighted(i0,params.mdAlpha,i1,params.mdBeta,params.mdGamma,out->image(),-1);
        break;
    }
}


const QString BlendImagesModel::_category = QString( "Image Operation" );

const QString BlendImagesModel::_model_name = QString( "Blend Images" );
