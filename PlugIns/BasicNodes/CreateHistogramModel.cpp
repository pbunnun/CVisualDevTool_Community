#include "CreateHistogramModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"
#include <math.h>

CreateHistogramModel::
CreateHistogramModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new CreateHistogramEmbeddedWidget ),
      _minPixmap( ":CreateHistogram.png" )
{ //ucharbin(mod(range)==0),ucharrange_max,ucharrange_min,intthic,intlinetype
    mpCVImageData = std::make_shared<CVImageData>(cv::Mat());
    cv::Mat init[3] = {cv::Mat(),cv::Mat(),cv::Mat()};
    mpArrayData_CVImage = std::make_shared<ArrayData<cv::Mat,3>>(init);
    mpSyncData = std::make_shared< SyncData >();

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect(mpEmbeddedWidget,&CreateHistogramEmbeddedWidget::comboBox_clicked_signal,this,&CreateHistogramModel::em_comboBox_clicked);

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miBinCount;
    intPropertyType.miMax = 256;
    QString propId = "bin_count";
    auto propBinCount = std::make_shared< TypedProperty< IntPropertyType > >( "Bin Count", propId, QVariant::Int, intPropertyType, "Operation" );
    mvProperty.push_back( propBinCount );
    mMapIdToProperty[ propId ] = propBinCount;

    DoublePropertyType doublePropertyType;
    doublePropertyType.mdValue = mParams.mdIntensityMax;
    doublePropertyType.mdMax = 255;
    propId = "intensity_max";
    auto propIntensityMax = std::make_shared< TypedProperty< DoublePropertyType > >( "Maximum Intensity", propId, QVariant::Double, doublePropertyType , "Operation");
    mvProperty.push_back( propIntensityMax );
    mMapIdToProperty[ propId ] = propIntensityMax;

    doublePropertyType.mdValue= mParams.mdIntensityMin;
    doublePropertyType.mdMax = 255;
    propId = "intensity_min";
    auto propIntensityMin = std::make_shared< TypedProperty< DoublePropertyType > >( "Minimum Intensity", propId, QVariant::Double, doublePropertyType, "Operation" );
    mvProperty.push_back( propIntensityMin );
    mMapIdToProperty[ propId ] = propIntensityMin;

    intPropertyType.miValue = mpEmbeddedWidget->get_comboBox_index();
    propId = "channels";
    auto propChannels = std::make_shared<TypedProperty<IntPropertyType>>("", propId, QVariant::Int, intPropertyType);
    mMapIdToProperty[ propId ] = propChannels;
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
        result = 2;
        break;

    default:
        break;
    }

    return result;
}


NodeDataType
CreateHistogramModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        if(portType == PortType::In)
        {
            return CVImageData().type();
        }
        else if(portType == PortType::Out)
        {
            if(mpEmbeddedWidget->get_comboBox_index() == 0)
            {
                return CVImageData().type();
            }
            else if(mpEmbeddedWidget->get_comboBox_index() == 1)
            {
                return ArrayData<cv::Mat,3>().type();
            }
        }
    }
    else if(portIndex == 1)
    {
        return SyncData().type();
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
CreateHistogramModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I == 0)
        {
            if(mpEmbeddedWidget->get_comboBox_index()==0)
            {
                return mpCVImageData;
            }
            else if(mpEmbeddedWidget->get_comboBox_index()==1)
            {
                return mpArrayData_CVImage;
            }
        }
        else if( I == 1)
        {
            return mpSyncData;
        }
    }
    return nullptr;
}

void
CreateHistogramModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData)
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            if(mpEmbeddedWidget->get_comboBox_index()==0)
            {
                processData(mpCVImageInData,mpCVImageData,mParams);
            }
            else if(mpEmbeddedWidget->get_comboBox_index()==1)
            {
                processData(mpCVImageInData,mpArrayData_CVImage,mParams);
            }
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
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
    cParams["channels"] = mpEmbeddedWidget->get_comboBox_index();
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
        v =  paramsObj[ "channels" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "channels" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
            typedProp->getData().miValue = v.toDouble();

            mpEmbeddedWidget->set_comboBox_index(v.toInt());
        }
    }
}

void
CreateHistogramModel::
setModelProperty( QString & id, const QVariant & value )
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
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

    if( mpCVImageInData )
    {
        const int channels = mpCVImageInData->image().channels();
        if(channels == 1)
        {
            processData( mpCVImageInData, mpCVImageData, mParams );
        }
        else if(channels == 3)
        {
            processData( mpCVImageInData, mpArrayData_CVImage, mParams );
        }
        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
CreateHistogramModel::
em_comboBox_clicked(int index)
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    if(mpCVImageInData)
    {
        if(index == 0)
        {
            processData(mpCVImageInData, mpCVImageData, mParams);
        }
        else if(index == 1)
        {
            processData(mpCVImageInData, mpArrayData_CVImage, mParams);
        }
        Q_EMIT dataUpdated(0);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
CreateHistogramModel::
processData( const std::shared_ptr<CVImageData> & in, std::shared_ptr<CVImageData> & out,
             const CreateHistogramParameters & params )
{
    const cv::Mat& in_image = in->image();
    if(in_image.empty() || (in_image.type()!=CV_8UC1 && in_image.type()!=CV_16UC1 && in_image.type()!=CV_32FC1))
    {
        return;
    }
    const float range[2] = {static_cast<float>( params.mdIntensityMin ),
                            static_cast<float>( params.mdIntensityMax+1 )}; //+1 to make it inclusive
    const float* pRange = &range[0];
    cv::calcHist(&in_image, 1, 0, cv::Mat(), out->image(), 1, & params.miBinCount, &pRange, true, false );
}

void
CreateHistogramModel::
processData( const std::shared_ptr<CVImageData> & in, std::shared_ptr<ArrayData<cv::Mat,3>> & out,
             const CreateHistogramParameters & params )
{
    const cv::Mat& in_image = in->image();
    if(in_image.empty() || (in_image.type()!=CV_8UC3 && in_image.type()!=CV_16UC3 && in_image.type()!=CV_32FC3))
    {
        return;
    }
    const float range[2] = {static_cast<float>( params.mdIntensityMin ),
                            static_cast<float>( params.mdIntensityMax+1 )}; //+1 to make it inclusive
    const float* pRange = &range[0];
    cv::Mat Temp[3];
    cv::split( in_image, &Temp[0] );
    for(int i=0; i<3; i++)
    {
       cv::calcHist( &Temp[i], 1, 0, cv::Mat(), out->array()[i], 1, &params.miBinCount, &pRange, true, false );
    }
}

const QString CreateHistogramModel::_category = QString( "Image Processing" );

const QString CreateHistogramModel::_model_name = QString( "Create Histogram" );
