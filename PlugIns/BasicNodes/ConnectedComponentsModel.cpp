#include "ConnectedComponentsModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include "qtvariantproperty.h"

ConnectedComponentsModel::
ConnectedComponentsModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":ConnectedComponents.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpIntegerData = std::make_shared< IntegerData >( int() );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"4", "8"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "connectivity";
    auto propConnectivity = std::make_shared< TypedProperty< EnumPropertyType > >( "Connectivity", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propConnectivity );
    mMapIdToProperty[ propId ] = propConnectivity;

    enumPropertyType.mslEnumNames = QStringList({"CV_16U", "CV_32S"});
    enumPropertyType.miCurrentIndex = 1;
    propId = "image_type";
    auto propImageType = std::make_shared< TypedProperty< EnumPropertyType > >( "Image Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation" );
    mvProperty.push_back( propImageType );
    mMapIdToProperty[ propId ] = propImageType;

    enumPropertyType.mslEnumNames = QStringList({"CCL_WU", "CCL_DEFAULT", "CCL_GRANA"});
    enumPropertyType.miCurrentIndex = 1;
    propId = "algorithm_type";
    auto propAlgorithmType = std::make_shared< TypedProperty< EnumPropertyType > >( "Algorithm Type", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation" );
    mvProperty.push_back( propAlgorithmType );
    mMapIdToProperty[ propId ] = propAlgorithmType;

    propId = "visualize";
    auto propVisualize = std::make_shared< TypedProperty < bool > >("Visualize", propId, QVariant::Bool, mParams.mbVisualize, "Display");
    mvProperty.push_back( propVisualize );
    mMapIdToProperty[ propId ] = propVisualize;
}

unsigned int
ConnectedComponentsModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
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
ConnectedComponentsModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portType == PortType::In)
    {
        return CVImageData().type();
    }
    else if(portType == PortType::Out)
    {
        switch(portIndex)
        {
        case 0:
            return CVImageData().type();

        case 1:
            return IntegerData().type();
        }
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
ConnectedComponentsModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        switch(I)
        {
        case 0:
            return mpCVImageData;

        case 1:
            return mpIntegerData;
        }
    }
    return nullptr;
}

void
ConnectedComponentsModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            processData( mpCVImageInData, mpCVImageData, mpIntegerData, mParams );
        }
    }

    updateAllOutputPorts();
}

QJsonObject
ConnectedComponentsModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["connectivity"] = mParams.miConnectivity;
    cParams["imageType"] = mParams.miImageType;
    cParams["algorithmType"] = mParams.miAlgorithmType;
    cParams["visualize"] = mParams.mbVisualize;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ConnectedComponentsModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "connectivity" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "connectivity" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miConnectivity = v.toInt();
        }
        v =  paramsObj[ "imageType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "image_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miImageType = v.toInt();
        }
        v =  paramsObj[ "algorithmType" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "algorithm_type" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miAlgorithmType = v.toInt();
        }
        v =  paramsObj[ "visualize" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "visualize" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
            typedProp->getData() = v.toBool();

            mParams.mbVisualize = v.toBool();
        }
    }
}

void
ConnectedComponentsModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "connectivity" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miConnectivity = 4;
            break;

        case 1:
            mParams.miConnectivity = 8;
            break;
        }
    }
    else if( id == "image_type" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miImageType = CV_16U;
            break;

        case 1:
            mParams.miImageType = CV_32S;
            break;
        }
    }
    else if( id == "algorithm_type" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < EnumPropertyType >>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miAlgorithmType = cv::CCL_WU;
            break;

        case 1:
            mParams.miAlgorithmType = cv::CCL_DEFAULT;
            break;

        case 2:
            mParams.miAlgorithmType = cv::CCL_GRANA;
            break;
        }
    }
    else if( id == "visualize" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < bool >>(prop);
        typedProp->getData() = value.toBool();

        mParams.mbVisualize = value.toBool();
    }
    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mpIntegerData, mParams );
        updateAllOutputPorts();
    }
}

void
ConnectedComponentsModel::
processData( const std::shared_ptr< CVImageData> & in, std::shared_ptr< CVImageData > & outImage,
             std::shared_ptr<IntegerData> &outInt, const ConnectedComponentsParameters & params )
{
    cv::Mat& in_image = in->image();
    if(in_image.channels()!=1 || in_image.empty())
    {
        return;
    }
    cv::Mat& out_image = outImage->image();
    cv::Mat Temp;
    double arr[2];
    cv::minMaxLoc(in_image,&arr[0],&arr[1]);
    bool isBinary = true;
    for(int i=0; i<in_image.rows; i++)
    {
        if(!isBinary)
        {
            break;
        }
        for(int j=0; j<in_image.cols; j++)
        {
            if(!isBinary)
            {
                break;
            }
            double value = static_cast<double>(in_image.at<uchar>(i,j));
            if(value!=arr[0] && value!=arr[1])
            {
                isBinary = false;
            }
        }
    }
    if(!isBinary)
    {
        return;
    }
    outInt->number() = cv::connectedComponents(in_image,
                                               Temp,
                                               params.miConnectivity,
                                               params.miImageType,
                                               params.miAlgorithmType);
    cv::resize(Temp,out_image,in_image.size());
    if(params.mbVisualize)
    {
        cv::normalize(out_image,out_image,0,255,cv::NORM_MINMAX,CV_8U);
    }
}


const QString ConnectedComponentsModel::_category = QString( "Image Processing" );

const QString ConnectedComponentsModel::_model_name = QString( "Connected Components" );
