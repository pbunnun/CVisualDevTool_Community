#include "RGBsetValueModel.hpp" //INCOMPLETE

#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

RGBsetValueModel::RGBsetValueModel()
    :PBNodeDataModel(_model_name,true),
     mpEmbeddedWidget(new RGBsetValueEmbeddedWidget()),
     _minPixmap(":RGBsetValue.png")
{
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &RGBsetValueEmbeddedWidget::button_clicked_signal, this, &RGBsetValueModel::em_button_clicked );

    UcharPropertyType ucharPropertyType;
    QString propId = "r_value";
    ucharPropertyType.mucValue = mParams.mucRvalue;
    auto propRvalue = std::make_shared<TypedProperty<UcharPropertyType>>("R Value",propId,QVariant::Int, ucharPropertyType);
    mvProperty.push_back(propRvalue);
    mMapIdToProperty[propId] = propRvalue;

    propId = "g_value";
    ucharPropertyType.mucValue = mParams.mucGvalue;
    auto propGvalue = std::make_shared<TypedProperty<UcharPropertyType>>("G Value",propId,QVariant::Int, ucharPropertyType);
    mvProperty.push_back(propGvalue);
    mMapIdToProperty[propId] = propGvalue;

    propId = "b_value";
    ucharPropertyType.mucValue = mParams.mucBvalue;
    auto propBvalue = std::make_shared<TypedProperty<UcharPropertyType>>("B Value",propId,QVariant::Int,ucharPropertyType);
    mvProperty.push_back(propBvalue);
    mMapIdToProperty[propId] = propBvalue;
}

unsigned int RGBsetValueModel::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch(portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result =1;
        break;
    default:
        break;
    }
    return result;
}

NodeDataType RGBsetValueModel::dataType(PortType,PortIndex) const
{
    return CVImageData().type();
}

std::shared_ptr<NodeData> RGBsetValueModel::outData(PortIndex)
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

void RGBsetValueModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if(nodeData)
    {
        auto d= std::dynamic_pointer_cast<CVImageData>(nodeData);
        if(d) //no image modification when initially setting in the data
        {
            mpCVImageInData = d;
            cv::Mat cvRGBsetImage = d->image().clone();
            mpCVImageData = std::make_shared<CVImageData>(cvRGBsetImage);
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject RGBsetValueModel::save() const
{
    QJsonObject modelJson =PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["R value"] = mParams.mucRvalue;
    cParams["G value"] = mParams.mucGvalue;
    cParams["B value"] = mParams.mucBvalue;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void RGBsetValueModel::restore(QJsonObject const &p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p["cParams"].toObject();
    if(!paramsObj.isEmpty())
    {
        QJsonValue v =paramsObj["R value"];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty["r_value"];
            auto typedProp =std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = v.toInt();
            mParams.mucRvalue = v.toInt();
        }
        v =paramsObj["G value"];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty["g_value"];
            auto typedProp =std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = v.toInt();
            mParams.mucGvalue = v.toInt();
        }
        v =paramsObj["B value"];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty["b_value"];
            auto typedProp =std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = v.toInt();
            mParams.mucBvalue = v.toInt();
        }
    }
}

void
RGBsetValueModel::
em_button_clicked( int )
{
   if(mpCVImageInData)
   {
       mpCVImageData = std::make_shared<CVImageData>(mpCVImageInData->image());
       Q_EMIT dataUpdated( 0 );
   }
}

void RGBsetValueModel::setModelProperty(QString &id, const QVariant & value)
{
    PBNodeDataModel::setModelProperty(id, value);
    if(!mMapIdToProperty.contains(id))
    {
        return;
    }
    auto prop = mMapIdToProperty[id];
    if(id=="r_value")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = value.toInt();
        mParams.mucRvalue = value.toInt();
    }
    else if(id=="g_value")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = value.toInt();
        mParams.mucGvalue = value.toInt();
    }
    else if(id=="b_value")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = value.toInt();
        mParams.mucBvalue = value.toInt();
    }

    if(mpCVImageData)
    {
        RGBsetValueProperties inProp;
        if(id=="r_value")
        {
            inProp.miChannel = 2;
            inProp.mucValue = mParams.mucRvalue;
        }
        else if(id=="g_value")
        {
            inProp.miChannel = 1;
            inProp.mucValue = mParams.mucGvalue;
        }
        else if(id=="b_value")
        {
            inProp.miChannel = 0;
            inProp.mucValue = mParams.mucBvalue;
        }
        cv::Mat cvRGBsetImage = processData(mpCVImageInData,inProp);
        mpCVImageData = std::make_shared<CVImageData>(cvRGBsetImage);

        Q_EMIT dataUpdated(0);
    }
}

cv::Mat RGBsetValueModel::processData(const std::shared_ptr<CVImageData> &p, const RGBsetValueProperties &prop)
{
    cv::Mat Output = p->image().clone();
    for(int i=0; i<Output.rows; i++)
    {
        for(int j=0; j<Output.cols; j++)
        {
            Output.at<cv::Vec3b>(i,j)[prop.miChannel] = cv::saturate_cast<uchar>(prop.mucValue);
        }
    }
    return Output;
}

const QString RGBsetValueModel::_category = QString("Image Operation");
const QString RGBsetValueModel::_model_name = QString("RGB Values");
