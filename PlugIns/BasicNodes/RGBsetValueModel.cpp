#include "RGBsetValueModel.hpp"

#include <QtCore/QDir>
#include <QDebug>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

RGBsetValueModel::RGBsetValueModel()
    :PBNodeDataModel(_model_name,true),
     _minPixmap(":RGBsetValue.png")
{
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
    if(mbEnable)
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
        if(d)
        {
            mpCVImageInData = d;
            cv::Mat cvRGBsetImage = d->image().clone();
            /*
            int row = cvRGBsetImage.rows;
            int col = cvRGBsetImage.cols;
            //if(cvRGBsetImage.isContinuous())
            //{
              //  col *= row;
                //row = 1;
            //}
            for(int i=0; i<row; i++)
            {
                for(int j=0; j<col; j++)
                {
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[0]=(uchar)mParams.mucBvalue;
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[1]=(uchar)mParams.mucGvalue;
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[2]=(uchar)mParams.mucRvalue;
                }
            }
            */
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
            typedProp->getData().mucValue = static_cast<uchar>(v.toInt());
            mParams.mucRvalue = static_cast<uchar>(v.toInt());
        }
        v =paramsObj["G value"];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty["g_value"];
            auto typedProp =std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = static_cast<uchar>(v.toInt());
            mParams.mucGvalue = static_cast<uchar>(v.toInt());
        }
        v =paramsObj["B value"];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty["b_value"];
            auto typedProp =std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
            typedProp->getData().mucValue = static_cast<uchar>(v.toInt());
            mParams.mucBvalue = static_cast<uchar>(v.toInt());
        }
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
        mParams.mucRvalue = static_cast<uchar>(value.toInt());
    }
    else if(id=="g_value")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = value.toInt();
        mParams.mucGvalue = static_cast<uchar>(value.toInt());
    }
    else if(id=="b_value")
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = value.toInt();
        mParams.mucBvalue = static_cast<uchar>(value.toInt());
    }

    if(mpCVImageData)
    {;
        cv::Mat cvRGBsetImage = mpCVImageData->image().clone();
        int row = cvRGBsetImage.rows;
        int col = cvRGBsetImage.cols;
        for(int i=0; i<row; i++)
        {
            for(int j=0; j<col; j++)
            {
                if(id=="r_value")
                {
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[0]=(uchar)mParams.mucBvalue;
                }
                else if(id=="g_value")
                {
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[1]=(uchar)mParams.mucGvalue;
                }
                else if(id=="b_value")
                {
                    cvRGBsetImage.at<cv::Vec3b>(i,j)[2]=(uchar)mParams.mucRvalue;
                }
            }
        }
        //*/
        mpCVImageData = std::make_shared<CVImageData>(cvRGBsetImage);
        Q_EMIT dataUpdated(0);
    }
}

const QString RGBsetValueModel::_category = QString("Image Operation");
const QString RGBsetValueModel::_model_name = QString("RGB Values");
