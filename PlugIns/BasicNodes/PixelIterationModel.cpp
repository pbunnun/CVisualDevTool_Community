#include "PixelIterationModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"


void PixIter::Iterate(cv::Mat &image, const cv::Scalar &colors, int* number, double* alpha, double* beta) const
{
    cv::Vec3b Temp(colors[0],colors[1],colors[2]);
    if(miIterKey == COUNT)
    {
        if(image.channels()==3)
        {
            *number = 0;
            for(int i=0; i<image.rows; i++)
            {
                for(int j=0; j<image.cols; j++)
                {
                    if(image.at<cv::Vec3b>(i,j) == Temp)
                    {
                        (*number)++;
                    }
                }
            }
        }
        else if(image.channels()==1)
        {
            *number = 0;
            for(int i=0; i<image.rows; i++)
            {
                for(int j=0; j<image.cols; j++)
                {
                    if(image.at<uchar>(i,j) == colors[0])
                    {
                        (*number)++;
                    }
                }
            }
        }
    }
    else if(miIterKey == BLANK)
    {
        *number = 0;
        for(int i=0; i<image.rows; i++)
        {
            for(int j=0; j<image.cols; j++)
            {
                if(image.at<cv::Vec3b>(i,j) == Temp)
                {
                    image.at<cv::Vec3b>(i,j) = cv::Vec3b::all(0);
                    (*number)++;
                }
            }
        }
    }
    else if(miIterKey == LINEAR)
    {
        for(int i=0; i<image.rows; i++)
        {
            for(int j=0; j<image.cols; j++)
            {
                image.at<cv::Vec3b>(i,j)
                = cv::Vec3b(*alpha*image.at<cv::Vec3b>(i,j)[0]+*beta,
                             *alpha*image.at<cv::Vec3b>(i,j)[1]+*beta,
                             *alpha*image.at<cv::Vec3b>(i,j)[2]+*beta);
            }
        }
    }
}


PixelIterationModel::
PixelIterationModel()
    : PBNodeDataModel( _model_name, true ),
      _minPixmap( ":PixelIteration.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpIntegerData = std::make_shared< IntegerData >( int() );

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"COUNT", "BLANK", "LINEAR"});
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "operation";
    auto propOperation = std::make_shared< TypedProperty< EnumPropertyType > >( "Operation", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propOperation );
    mMapIdToProperty[ propId ] = propOperation;

    UcharPropertyType ucharPropertyType;
    for(int i=0; i<3; i++)
    {
        ucharPropertyType.mucValue = mParams.mucColorInput[i];
        QString colorInput = QString::fromStdString("Input Color "+color[i]);
        propId = QString("color_input_%1").arg(i);
        auto propColorInput = std::make_shared< TypedProperty< UcharPropertyType > >( colorInput, propId, QVariant::Int , ucharPropertyType, "Operation" );
        mvProperty.push_back( propColorInput );
        mMapIdToProperty[ propId ] = propColorInput;
    }
}

unsigned int
PixelIterationModel::
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
PixelIterationModel::
dataType(PortType portType, PortIndex portIndex) const
{
    if(portIndex==0)
    {
        return CVImageData().type();
    }
    else if(portIndex==1)
    {
        if(portType==PortType::In)
        {
            return CVScalarData().type();
        }
        else if(portType==PortType::Out)
        {
            return IntegerData().type();
        }
    }
    return NodeDataType();
}


std::shared_ptr<NodeData>
PixelIterationModel::
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
        return nullptr;
    }
    else
        return nullptr;
}

void
PixelIterationModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        if(portIndex == 0)
        {
            auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
            if (d)
            {
                mpCVImageInData = d;
            }
        }
        else if( portIndex == 1 )
        {
            auto d= std::dynamic_pointer_cast<CVScalarData>(nodeData);
            if (d)
            {
                mpCVScalarInData = d;
            }
        }
        if( mpCVImageInData )
        {
            if( mpCVScalarInData )
            {
                overwrite(mpCVScalarInData,mParams);
            }
            processData(mpCVImageInData,mpCVImageData,mpIntegerData,mParams);
        }
    }

    updateAllOutputPorts();
}

QJsonObject
PixelIterationModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["operation"] = mParams.miOperation;
    for(int i=0; i<3; i++)
    {
        cParams[QString("colorInput%1").arg(i)] = mParams.mucColorInput[i];
    }
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
PixelIterationModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue v = paramsObj[ "operation" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "operation" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miOperation = v.toInt();
        }
        for(int i=0; i<3; i++)
        {
            v = paramsObj[QString("colorInput%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("color_input_%1").arg(i)];
                auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
                typedProp->getData().mucValue = v.toInt();

                mParams.mucColorInput[i] = v.toInt();
            }
        }
    }
}

void
PixelIterationModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    if( id == "operation" )
    {
        auto typedProp = std::static_pointer_cast<TypedProperty<EnumPropertyType>>(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        mParams.miOperation = value.toInt();
    }
    for(int i=0; i<3; i++)
    {
        if( id == QString("color_input_%1").arg(i) )
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
            typedProp->getData().mucValue = value.toInt();

            mParams.mucColorInput[i] = value.toInt();
        }
    }
    if( mpCVImageInData )
    {
        processData( mpCVImageInData, mpCVImageData, mpIntegerData, mParams );

        updateAllOutputPorts();
    }
}

void
PixelIterationModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr<CVImageData> & out,
            std::shared_ptr<IntegerData> &outInt, const PixelIterationParameters & params )
{
    out->set_image(in->image());
    cv::Scalar colors(params.mucColorInput[0],
                      params.mucColorInput[1],
                      params.mucColorInput[2]);
    PixIter It(params.miOperation);
    It.Iterate(out->image(),colors,&(outInt->number()));
}

void
PixelIterationModel::
overwrite(std::shared_ptr<CVScalarData> &in, PixelIterationParameters &params)
{
    cv::Scalar& in_scalar = in->scalar();
    for(int i=0; i<3; i++)
    {
        auto prop = mMapIdToProperty[QString("color_input_%1").arg(i)];
        auto typedProp = std::static_pointer_cast<TypedProperty<UcharPropertyType>>(prop);
        typedProp->getData().mucValue = in_scalar[i];
        params.mucColorInput[i] = in_scalar[i];
    }
    in.reset();
}

const std::string PixelIterationModel::color[3] = {"B", "G", "R"};

const QString PixelIterationModel::_category = QString( "Image Modification" );

const QString PixelIterationModel::_model_name = QString( "Pixel Iteration" );
