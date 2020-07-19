#include "DrawHistogramModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"
#include <math.h>

DrawHistogramModel::
DrawHistogramModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":DrawHistogram.png" )
{
    mpCVImageData = std::make_shared< CVImageData >(cv::Mat());
    mpSyncData = std::make_shared< SyncData >();

    EnumPropertyType enumPropertyType; //9
    enumPropertyType.mslEnumNames = QStringList({"NORM_L1", "NORM_L2", "NORM_INF", "NORM_L2SQR", "NORM_MINMAX", "NORM_HAMMING", "NORM_HAMMING2", "NORM_RELATIVE", "NORM_TYPE_MASK"});
    enumPropertyType.miCurrentIndex = 4;
    QString propId = "norm_type";
    auto propNormType = std::make_shared< TypedProperty< EnumPropertyType >>("Norm Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propNormType );
    mMapIdToProperty[ propId ] = propNormType;

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared< TypedProperty< IntPropertyType > >( "Line Thickness", propId, QVariant::Int, intPropertyType , "Display");
    mvProperty.push_back( propLineThickness );
    mMapIdToProperty[ propId ] = propLineThickness;

    enumPropertyType.mslEnumNames = QStringList({"LINE_8", "LINE_4", "LINE_AA"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "line_type";
    auto propLineType = std::make_shared< TypedProperty< EnumPropertyType >>("Line Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Display");
    mvProperty.push_back( propLineType );
    mMapIdToProperty[ propId ] = propLineType;

    propId = "draw_endpoints";
    auto propDrawEndpoints= std::make_shared<TypedProperty<bool>>("Draw Endpoints", propId, QVariant::Bool, mParams.mbDrawEndpoints, "Display");
    mvProperty.push_back( propDrawEndpoints );
    mMapIdToProperty[ propId ] = propDrawEndpoints;

    propId = "enable_b";
    auto propEnableB = std::make_shared<TypedProperty<bool>>("Enable B", propId, QVariant::Bool, mParams.mbEnableB, "Display");
    mvProperty.push_back( propEnableB );
    mMapIdToProperty[ propId ] = propEnableB;

    propId = "enable_g";
    auto propEnableG= std::make_shared<TypedProperty<bool>>("Enable G", propId, QVariant::Bool, mParams.mbEnableG, "Display");
    mvProperty.push_back( propEnableG );
    mMapIdToProperty[ propId ] = propEnableG;

    propId = "enable_r";
    auto propEnableR= std::make_shared<TypedProperty<bool>>("Enable R", propId, QVariant::Bool, mParams.mbEnableR, "Display");
    mvProperty.push_back( propEnableR );
    mMapIdToProperty[ propId ] = propEnableR;
}

unsigned int
DrawHistogramModel::
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
DrawHistogramModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        return CVImageData().type();
    }
    if(portIndex == 1)
    {
        if(portType == PortType::In)
        {
            return ArrayData<cv::Mat,3>().type();
        }
        else if(portType == PortType::Out)
        {
            return SyncData().type();
        }
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
DrawHistogramModel::
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
DrawHistogramModel::
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
                processData( mpCVImageInData, mpCVImageData, mParams );
            }
        }
        if(portIndex == 1)
        {
            auto d = std::dynamic_pointer_cast<ArrayData<cv::Mat,3>>(nodeData);
            if(d)
            {
                mpArrayInData_CVImage = d;
                processData( mpArrayInData_CVImage, mpCVImageData, mParams);
            }
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
DrawHistogramModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["normType"] = mParams.miNormType;
    cParams["lineThickness"] = mParams.miLineThickness;
    cParams["lineType"] = mParams.miLineType;
    cParams["drawEndpoints"] = mParams.mbDrawEndpoints;
    cParams["enableB"] = mParams.mbEnableB;
    cParams["enableG"] = mParams.mbEnableG;
    cParams["enableR"] = mParams.mbEnableR;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
DrawHistogramModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v =  paramsObj[ "normType" ];
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
        v = paramsObj[ "drawEndpoints" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "draw_endpoints" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbDrawEndpoints = v.toBool();
        }
        v = paramsObj[ "enableB" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "enable_b" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbEnableB = v.toBool();
        }
        v = paramsObj[ "enableG" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "enable_g" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbEnableG = v.toBool();
        }
        v = paramsObj[ "enableR" ];
        if(!v.isUndefined())
        {
            auto prop = mMapIdToProperty[ "enable_r" ];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbEnableR = v.toBool();
        }
    }
}

void
DrawHistogramModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "norm_type" ) //{"NORM_L1", "NORM_L2", "NORM_INF", "NORM_L2SQR", "NORM_MINMAX", "NORM_HAMMING", "NORM_HAMMING2", "NORM_RELATIVE", "NORM_TYPE_MASK"}
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();
        switch(value.toInt()) //Only NORM_MINMAX is currently functional
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
    else if(id=="draw_endpoints")
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbDrawEndpoints = value.toBool();
    }
    else if(id=="enable_b")
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbEnableB = value.toBool();
    }
    else if(id=="enable_g")
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbEnableG = value.toBool();
    }
    else if(id=="enable_r")
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbEnableR = value.toBool();
    }
    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mParams );
    }
    else if( mpArrayInData_CVImage )
    {
        processData( mpArrayInData_CVImage, mpCVImageData, mParams );
    }
    Q_EMIT dataUpdated(0);
}

void
DrawHistogramModel::
processData( const std::shared_ptr<CVImageData> & in, std::shared_ptr<CVImageData> & out,
             const DrawHistogramParameters & params )
{
    cv::Mat& in_image = in->image();
    if(in_image.empty() || in_image.type()!=CV_32FC1 ||
       in_image.cols != 1 || in_image.rows > 256)
    {
        return;
    }
    cv::Mat& out_image = out->image();
    cv::Mat Temp;
    out->set_image(cv::Mat::zeros(512,512,CV_8UC1)); //hard coding
    const int binCount = in_image.rows;
    const double binSize = static_cast<double>(out_image.cols)/binCount;

    cv::normalize(in_image, Temp, 0, out_image.rows, params.miNormType, -1 );
    std::vector< cv::Point > vPoint = {};
    vPoint.reserve(binCount+2);
    if(params.mbDrawEndpoints)
    {
        vPoint.push_back( cv::Point( 0, out_image.rows ) );
    }
    for(int i = 0; i < binCount; i++)
    {
        vPoint.push_back(cv::Point(static_cast<int>((i+0.5)*binSize),
                                   out_image.rows - cvRound(Temp.at<float>(i))));
    }
    if(params.mbDrawEndpoints)
    {
        vPoint.push_back( cv::Point( out_image.cols , out_image.rows ));
    }
    cv::Scalar color(255);
    std::vector< std::vector< cv::Point > > vvPoint = { vPoint };
    cv::polylines( out_image, vvPoint, false, color, params.miLineThickness, params.miLineType);
}

void
DrawHistogramModel::
processData(const std::shared_ptr<ArrayData<cv::Mat, 3> > &in, std::shared_ptr<CVImageData> &out,
            const DrawHistogramParameters &params)
{
    cv::Mat* in_array = in->array();
    const int size = in->size();
    for(int i=0; i<size; i++)
    {
        if(in_array[i].empty() || in_array[i].type()!=CV_32FC1 ||
                in_array[i].cols != 1 || in_array[i].rows > 256)
        {
            return;
        }
    }
    cv::Mat& out_image = out->image();
    out->set_image(cv::Mat::zeros(512,512,CV_8UC3)); //hard coding
    const bool enableDisplay[3] = {params.mbEnableB,
                                   params.mbEnableG,
                                   params.mbEnableR};
    const cv::Scalar colors[3] = {cv::Scalar(255,0,0),
                                  cv::Scalar(0,255,0),
                                  cv::Scalar(0,0,255)};
    const int binCount = in_array[0].rows;
    const double binSize = static_cast<double>(out_image.cols)/binCount;

    for(int i=0; i<size; i++)
    {
        if(enableDisplay[i])
        {
            cv::Mat Temp;
            cv::normalize(in_array[i],
                          Temp,
                          0,
                          out_image.rows,
                          params.miNormType,
                          -1 );
            std::vector< cv::Point > vPoint = {};
            vPoint.reserve(binCount+2);
            if(params.mbDrawEndpoints)
            {
                vPoint.push_back( cv::Point( 0, out_image.rows ) );
            }
            for(int j=0; j<binCount; j++)
            {
                vPoint.push_back(cv::Point(static_cast<int>((j+0.5)*binSize),
                                           out_image.rows-cvRound(Temp.at<float>(j))));
            }
            if(params.mbDrawEndpoints)
            {
                vPoint.push_back( cv::Point( out_image.cols , out_image.rows ));
            }
            std::vector< std::vector< cv::Point > > vvPoint = { vPoint };
            cv::polylines( out_image,
                           vvPoint,
                           false,
                           colors[i],
                           params.miLineThickness,
                           params.miLineType);
        }
    }
}

const QString DrawHistogramModel::_category = QString( "Image Analysis" );

const QString DrawHistogramModel::_model_name = QString( "Draw Histogram" );
