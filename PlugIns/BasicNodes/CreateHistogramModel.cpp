#include "CreateHistogramModel.hpp"

#include <QtCore/QEvent> //???
#include <QtCore/QDir> //???
#include <QDebug> //for debugging using qDebug()

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"
#include <math.h>

CreateHistogramModel::
CreateHistogramModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":CreateHistogram.png" )
{ //ucharbin(mod(range)==0),ucharrange_max,ucharrange_min,intthic,intlinetype
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miBinCount;
    intPropertyType.miMax = 256;
    QString propId = "bin_count";
    auto propBinCount = std::make_shared< TypedProperty< IntPropertyType > >( "Bin Count", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propBinCount );
    mMapIdToProperty[ propId ] = propBinCount;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdIntensityMax;
    doublePropertyType.mdMax = 255;
    propId = "intensity_max";
    auto propIntensityMax = std::make_shared< TypedProperty< DoublePropertyType > >( "Maximum Intensity", propId, QVariant::Double, doublePropertyType );
    mvProperty.push_back( propIntensityMax );
    mMapIdToProperty[ propId ] = propIntensityMax;

    doublePropertyType.mdValue= mParams.mdIntensityMin;
    doublePropertyType.mdMax = 255;
    propId = "intensity_min";
    auto propIntensityMin = std::make_shared< TypedProperty< DoublePropertyType > >( "Minimum Intensity", propId, QVariant::Double, doublePropertyType );
    mvProperty.push_back( propIntensityMin );
    mMapIdToProperty[ propId ] = propIntensityMin;

    EnumPropertyType enumPropertyType; //9
    enumPropertyType.mslEnumNames = QStringList({"NORM_L1", "NORM_L2", "NORM_INF", "NORM_L2SQR", "NORM_MINMAX", "NORM_HAMMING", "NORM_HAMMING2", "NORM_RELATIVE", "NORM_TYPE_MASK"});
    enumPropertyType.miCurrentIndex = 4;
    propId = "norm_type";
    auto propNormType = std::make_shared< TypedProperty< EnumPropertyType >>("Line Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propNormType );
    mMapIdToProperty[ propId ] = propNormType;

    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared< TypedProperty< IntPropertyType > >( "Line Thickness", propId, QVariant::Int, intPropertyType );
    mvProperty.push_back( propLineThickness );
    mMapIdToProperty[ propId ] = propLineThickness;

    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "line_type";
    auto propLineType = std::make_shared< TypedProperty< EnumPropertyType >>("Line Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propLineType );
    mMapIdToProperty[ propId ] = propLineType;
}

unsigned int
CreateHistogramModel::
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
CreateHistogramModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
CreateHistogramModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
CreateHistogramModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            cv::Mat cvHistogramImage = processData(mParams,d);
            mpCVImageData = std::make_shared<CVImageData>(cvHistogramImage);
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
CreateHistogramModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["binCount"] = mParams.miBinCount;
    cParams["intensityMax"] = mParams.mdIntensityMax;
    cParams["intensityMin"] = mParams.mdIntensityMin;
    cParams["normType"] = mParams.miNormType;
    cParams["lineThickness"] = mParams.miLineThickness;
    cParams["lineType"] = mParams.miLineType;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
CreateHistogramModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "binCount" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "bin_count" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miBinCount = v.toInt();
        }
        v =  paramsObj[ "intensityMax" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "intensity_max" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdIntensityMax = v.toDouble();
        }
        v =  paramsObj[ "intensityMin" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "intensity_min" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
            typedProp->getData().mdValue = v.toDouble();

            mParams.mdIntensityMin = v.toDouble();
        }
        v =  paramsObj[ "normType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "norm_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miNormType = v.toInt();
        }
        v =  paramsObj[ "lineThickness" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_thickness" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toInt();

            mParams.miLineThickness = v.toInt();
        }
        v = paramsObj[ "lineType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "line_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miLineType = v.toInt();
        }
    }
}

void
CreateHistogramModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "bin_count" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miBinCount = value.toInt();
    }
    else if( id == "intensity_max" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdIntensityMax = value.toDouble();
    }
    else if( id == "intensity_min" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< DoublePropertyType > >( prop );
        typedProp->getData().mdValue = value.toDouble();

        mParams.mdIntensityMin = value.toDouble();
    }
    else if( id == "norm_type" ) //{"NORM_L1", "NORM_L2", "NORM_INF", "NORM_L2SQR", "NORM_MINMAX", "NORM_HAMMING", "NORM_HAMMING2", "NORM_RELATIVE", "NORM_TYPE_MASK"}
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        switch(value.toInt())
        {
        case 0:
            mParams.miNormType = cv::NORM_L1;
            break;

        case 1:
            mParams.miNormType = cv::NORM_L2;
            break;

        case 2:
            mParams.miNormType = cv::NORM_INF;
            break;

        case 3:
            mParams.miNormType = cv::NORM_L2SQR;
            break;

        case 4:
            mParams.miNormType = cv::NORM_MINMAX;
            break;

        case 5:
            mParams.miNormType = cv::NORM_HAMMING;
            break;

        case 6:
            mParams.miNormType = cv::NORM_HAMMING2;
            break;

        case 7:
            mParams.miNormType = cv::NORM_RELATIVE;
            break;

        case 8:
            mParams.miNormType = cv::NORM_TYPE_MASK;
            break;
        }
    }
    else if( id == "line_thickness" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miLineThickness = value.toInt();
    }
    else if( id == "line_type" ) //{"NORM_L1", "NORM_L2", "NORM_INF", "NORM_L2SQR", "NORM_MINMAX", "NORM_HAMMING", "NORM_HAMMING2", "NORM_RELATIVE", "NORM_TYPE_MASK"}
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
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
        cv::Mat cvHistogramImage = processData(mParams,mpCVImageInData);
        mpCVImageData = std::make_shared<CVImageData>(cvHistogramImage);
        Q_EMIT dataUpdated(0);
    }
}

cv::Mat CreateHistogramModel::processData(const CreateHistogramParameters &mParams, const std::shared_ptr<CVImageData> &p)
{
    float range[2] = {static_cast<float>(mParams.mdIntensityMin),static_cast<float>(mParams.mdIntensityMax+1)}; //+1 to make it inclusive
    double binSize = static_cast<double>((range[1]-range[0])/mParams.miBinCount);
    const float* pRange = &range[0];
    if(p->image().channels()==1)
    {
        cv::Mat Output(256,256,CV_8UC1,cv::Scalar::all(0));
        cv::Mat cvChannelSplit = p->image();
        cv::Mat cvHistogramSplit;
        cv::calcHist(&cvChannelSplit,1,0,cv::Mat(),cvHistogramSplit,1,&mParams.miBinCount,&pRange,true,false);
        cv::normalize(cvHistogramSplit,cvHistogramSplit,0,Output.rows,mParams.miNormType,-1);
        std::vector<cv::Point> vPoint = {cv::Point(0,Output.rows)};
        for(int j=0; j<mParams.miBinCount; j++)
        {
            vPoint.push_back(cv::Point((j+0.5)*binSize,Output.rows-cvRound(cvHistogramSplit.at<float>(j))));
        }
        vPoint.push_back(cv::Point(Output.cols,Output.rows));
        cv::Scalar color(255);
        std::vector<std::vector<cv::Point>> vvPoint = {vPoint};
        cv::polylines(Output,vvPoint,false,color,mParams.miLineThickness,mParams.miLineType);
        return Output;
    }
    else
    {
        cv::Mat Output(256,256,CV_8UC3,cv::Scalar::all(0));
        std::array<cv::Mat,3> cvBGRChannelSplit;
        std::array<cv::Mat,3> cvHistogramSplit;
        cv::split(p->image(),cvBGRChannelSplit);
        for(int i=0; i<static_cast<int>(cvBGRChannelSplit.size()); i++)
        {
            cv::calcHist(&cvBGRChannelSplit[i],1,0,cv::Mat(),cvHistogramSplit[i],1,&mParams.miBinCount,&pRange,true,false);
            cv::normalize(cvHistogramSplit[i],cvHistogramSplit[i],0,Output.rows,mParams.miNormType,-1);
            std::vector<cv::Point> vPoint = {cv::Point(0,Output.rows)};
            for(int j=0; j<mParams.miBinCount; j++)
            {
                vPoint.push_back(cv::Point((j+0.5)*binSize,Output.rows-cvRound(cvHistogramSplit[i].at<float>(j))));
            }
            vPoint.push_back(cv::Point(Output.cols,Output.rows));
            cv::Scalar color(0,0,0);
            color[i] = 255;
            std::vector<std::vector<cv::Point>> vvPoint = {vPoint};
            cv::polylines(Output,vvPoint,false,color,mParams.miLineThickness,mParams.miLineType);
            return Output;
        }
    }
    return cv::Mat();
}

const QString CreateHistogramModel::_category = QString( "Image Operation" );

const QString CreateHistogramModel::_model_name = QString( "Create Histogram" );
