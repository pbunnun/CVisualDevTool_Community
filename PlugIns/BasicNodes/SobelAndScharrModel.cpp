#include "SobelAndScharrModel.hpp"

#include <QtCore/QEvent> //???
#include <QtCore/QDir> //???
#include <QDebug> //for debugging using qDebug()

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

SobelAndScharrModel::
SobelAndScharrModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget(new SobelAndScharrEmbeddedWidget()),
      _minPixmap( ":SobelAndScharr.png" )
{
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &SobelAndScharrEmbeddedWidget::checkbox_checked_signal, this, &SobelAndScharrModel::em_checkbox_checked );

    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    IntPropertyType intPropertyType;
    QString propId = "x_order";
    intPropertyType.miValue = mParams.miOrderX;
    auto propOrderX = std::make_shared< TypedProperty< IntPropertyType > >( "X order", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propOrderX );
    mMapIdToProperty[ propId ] = propOrderX;

    intPropertyType.miValue = mParams.miOrderY;
    propId = "y_order";
    auto propOrderY = std::make_shared< TypedProperty< IntPropertyType > >( "Y order", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propOrderY );
    mMapIdToProperty[ propId ] = propOrderY;

    intPropertyType.miValue = mParams.miKernelSize;
    propId = "kernel_size";
    auto propKernelSize = std::make_shared< TypedProperty< IntPropertyType > >( "Kernel Size", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propKernelSize );
    mMapIdToProperty[ propId ] = propKernelSize;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdScale;
    propId = "scale";
    auto propScale = std::make_shared< TypedProperty <DoublePropertyType>>("Scale", propId,QVariant::Double, doublePropertyType);
    mvProperty.push_back( propScale );
    mMapIdToProperty[ propId ] = propScale;

    doublePropertyType.mdValue = mParams.mdDelta;
    propId = "delta";
    auto propDelta = std::make_shared< TypedProperty <DoublePropertyType>>("Delta", propId,QVariant::Double, doublePropertyType);
    mvProperty.push_back(propDelta);
    mMapIdToProperty[ propId ] = propDelta;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList( {"DEFAULT", "CONSTANT", "REPLICATE", "REFLECT", "WRAP", "TRANSPARENT", "ISOLATED"} );
    enumPropertyType.miCurrentIndex = 0;
    propId = "border_type";
    auto propBorderType = std::make_shared< TypedProperty< EnumPropertyType > >( "Border Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType );
    mvProperty.push_back( propBorderType );
    mMapIdToProperty[ propId ] = propBorderType;
}

unsigned int
SobelAndScharrModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 3;
        break;

    default:
        break;
    }

    return result;
}


NodeDataType
SobelAndScharrModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
SobelAndScharrModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        switch(I)
        {
        case 0:
            return mpCVImageData;

        case 1:
            return mpCVImageDataX;

        case 2:
            return mpCVImageDataY;

        default:
            return nullptr;
        }
    }
    else
        return nullptr;
}

void
SobelAndScharrModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            SobelAndScharrProperties inProp;
            inProp.mpPropertyWidget = mpEmbeddedWidget;
            std::vector<cv::Mat> cvSobelAndScharrImage = processData(mParams,d,inProp);
            mpCVImageData = std::make_shared<CVImageData>(cvSobelAndScharrImage[0]);
            mpCVImageDataX = std::make_shared<CVImageData>(cvSobelAndScharrImage[1]);
            mpCVImageDataY = std::make_shared<CVImageData>(cvSobelAndScharrImage[2]);
        }
    }
    updateAllOutputPorts();
}

QJsonObject
SobelAndScharrModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["orderX"] = mParams.miOrderX;
    cParams["orderY"] = mParams.miOrderY;
    cParams["kernelSize"] = mParams.miKernelSize;
    cParams["scale"] = mParams.mdScale;
    cParams["delta"] = mParams.mdDelta;
    cParams["border_type"] = mParams.miBorderType;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
SobelAndScharrModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "orderX" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "order_x" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miOrderX = v.toInt();
        }
        v =  paramsObj[ "orderY" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "order_y" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miOrderY = v.toInt();
        }
        v =  paramsObj[ "kernelSize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "kernel_size" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miKernelSize = v.toInt();
        }
        v = paramsObj[ "scale" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "scale" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType >>(prop);
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdScale = v.toDouble();
        }
        v = paramsObj[ "delta" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "delta" ];
            auto typedProp  = std::static_pointer_cast< TypedProperty< DoublePropertyType >>(prop);
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdDelta = v.toDouble();
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

void SobelAndScharrModel::em_checkbox_checked(int)
{
    if(mpCVImageInData)
    {
        QString id("kernel_size");
        setModelProperty(id,mParams.miKernelSize);
        updateAllOutputPorts();
    }
}

void
SobelAndScharrModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if(id=="order_x")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miValue = value.toInt();
        mParams.miOrderX = value.toInt();
    }
    else if(id=="order_y")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
        typedProp->getData().miValue = value.toInt();
        mParams.miOrderY = value.toInt();
    }
    else if( id == "kernel_size" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        int kSize =  value.toInt();
        bool adjValue = false;
        if( kSize%2==0 )
        {
            if(kSize==typedProp->getData().miMax)
            {
                kSize -= 1;
            }
            else
            {
                kSize += 1;
            }
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miValue = kSize;
            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            if(kSize==3)
            {
                mpEmbeddedWidget->change_enable_checkbox(true);
            }
            else
            {
                mpEmbeddedWidget->change_check_checkbox(Qt::Unchecked);
                mpEmbeddedWidget->change_enable_checkbox(false);
            }
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = kSize;
            mParams.miKernelSize = kSize;
        }
    }
    else if( id == "scale" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();
        mParams.mdScale = value.toDouble();
    }
    else if( id == "delta" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();
        mParams.mdDelta = value.toDouble();
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
        case 4: // WRAP (BUG)
            mParams.miBorderType = cv::BORDER_WRAP;
            break;
        case 5: // TRANSPARENT (BUG)
            mParams.miBorderType = cv::BORDER_TRANSPARENT;
            break;
        case 6: // ISOLATED
            mParams.miBorderType = cv::BORDER_ISOLATED;
            break;
        }
    }
    if( mpCVImageInData )
    {
        SobelAndScharrProperties inProp;
        inProp.mpPropertyWidget = mpEmbeddedWidget;
        std::vector<cv::Mat> cvSobelAndScharrImage = processData(mParams,mpCVImageInData,inProp);
        mpCVImageData = std::make_shared<CVImageData>(cvSobelAndScharrImage[0]);
        mpCVImageDataX = std::make_shared<CVImageData>(cvSobelAndScharrImage[1]);
        mpCVImageDataY = std::make_shared<CVImageData>(cvSobelAndScharrImage[2]);
        updateAllOutputPorts();
    }
}

std::vector<cv::Mat> SobelAndScharrModel::processData(const SobelAndScharrParameters &mParams, const std::shared_ptr<CVImageData> &p, SobelAndScharrProperties &prop)
{
    std::vector<cv::Mat> Output(3);
    cv::Mat Temp[3];
    if(p->image().channels()==1)
    {
        Temp[0] = p->image().clone();
    }
    else
    {
        cv::cvtColor(p->image(),Temp[0],cv::COLOR_BGR2GRAY);
    }
    if(prop.mpPropertyWidget->checkbox_is_checked())
    {
        cv::Scharr(p->image(),Temp[1],CV_16S,mParams.miOrderX,0,mParams.mdScale,mParams.mdDelta,mParams.miBorderType);
        cv::Scharr(p->image(),Temp[2],CV_16S,0,mParams.miOrderY,mParams.mdScale,mParams.mdDelta,mParams.miBorderType);
    }
    else
    {
        cv::Sobel(p->image(),Temp[1],CV_16S,mParams.miOrderX,0,mParams.miKernelSize,mParams.mdScale,mParams.mdDelta,mParams.miBorderType);
        cv::Sobel(p->image(),Temp[2],CV_16S,0,mParams.miOrderY,mParams.miKernelSize,mParams.mdScale,mParams.mdDelta,mParams.miBorderType);
    }
    cv::convertScaleAbs(Temp[1],Output[1]);
    cv::convertScaleAbs(Temp[2],Output[2]);
    cv::addWeighted(Output[1],0.5,Output[2],0.5,0,Output[0]);
    return Output;
}

const QString SobelAndScharrModel::_category = QString( "Image Operation" );

const QString SobelAndScharrModel::_model_name = QString( "Sobel and Scharr" );
