#include "GetAffineTransformModel.hpp"

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>

GetAffineTransformModel::
GetAffineTransformModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new GetAffineTransformationEmbeddedWidget ),
      _minPixmap( ":GetAffineTransform.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpSyncData = std::make_shared< SyncData >();

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect(mpEmbeddedWidget,&GetAffineTransformationEmbeddedWidget::button_clicked_signal,this,&GetAffineTransformModel::em_button_clicked);

}

unsigned int
GetAffineTransformModel::
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
GetAffineTransformModel::
dataType(PortType, PortIndex portIndex) const
{
    if(portIndex == 0)
        return CVImageData().type();
    else
        return SyncData().type();
}

std::shared_ptr<NodeData>
GetAffineTransformModel::
outData(PortIndex I)
{
    if( isEnable() )
    {
        if(I==0)
            return mpCVImageData;
        else if( I==1 )
            return mpSyncData;
    }
    return nullptr;
}

void
GetAffineTransformModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    if( nodeData )
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );
        if( d )
        {
            mpCVImageInData = d;
            this->mbNewImage = true;
            processData( mpCVImageInData, mpCVImageData );
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated( 0 );
}

void
GetAffineTransformModel::
em_button_clicked()
{
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    processData(mpCVImageInData,mpCVImageData);
    Q_EMIT dataUpdated(0);
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void
GetAffineTransformModel::
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > &)
{
    cv::Mat& in_image = in->image();
    mpEmbeddedWidget->enable_recreate_button(!in_image.empty());
    if(in_image.empty() || in_image.type()!=CV_8UC1 || in_image.type()!=CV_8UC3)
    {
        return;
    }
    cv::Mat Temp;
    if(in_image.channels() == 1)
    {
        cv::cvtColor(in_image,Temp,cv::COLOR_GRAY2BGR);
    }
    else
    {
        Temp = in_image.clone();
    }
    InputEventHandler inputHandler(Temp,mQSWindowName);
    connect(&inputHandler,&InputEventHandler::mousePressSignal,this,&GetAffineTransformModel::onMousePressed);
    connect(&inputHandler,&InputEventHandler::mouseReleaseSignal,this,&GetAffineTransformModel::onMouseReleased);
    connect(&inputHandler,&InputEventHandler::mouseMoveSignal,this,&GetAffineTransformModel::onMouseMoved);
    connect(this,&GetAffineTransformModel::onImageReady,&inputHandler,&InputEventHandler::imageUpdatedInput);
    inputHandler.activate();
}

void
GetAffineTransformModel::
onMousePressed(QMouseEvent* event, cv::Mat& image)
{
    if(mbNewImage)
    {
        mbNewImage = false;
        iterator = 0;
        canvas = image.clone();
        save[0] = image;
        for(uint i = 1; i<4; i++)
        {
            save[i] = cv::Mat();
        }
        holdingPoint = cv::Point();
        for(uint i=0; i<2; i++)
        {
            for(uint j=0; j<3; j++)
            {
                warpArray[i][j] = cv::Point2f();
            }
        }
        cancel = false;
        holding = false;
    }
    if(iterator == 3 && event->buttons()!=Qt::RightButton)
    {
        return;
    }
    if(event->buttons()==Qt::LeftButton)
    {
        holdingPoint = cv::Point2f(event->x(),event->y());
        holding = true;
    }
    else if(event->buttons()==Qt::RightButton)
    {
        if(holding)
        {
            holding = false;
            canvas = save[iterator].clone();
            cancel = true;
        }
        else
        {
            if(iterator>0)
            {
                if(iterator==3)
                {
                    image = save[0].clone();
                }
                canvas = save[iterator-1].clone();
                iterator--;
            }
        }
        Q_EMIT onImageReady(canvas);
    }
}

void
GetAffineTransformModel::
onMouseReleased(QMouseEvent* event, cv::Mat& image)
{
    if(event->button()==Qt::LeftButton && iterator!=3)
    {
        holding = false;
        warpArray[1][iterator] = cv::Point2f(event->x(),event->y());
        warpArray[0][iterator] = holdingPoint;
        holdingPoint = cv::Point2f();
        if(!cancel)
        {
            save[iterator+1] = canvas;
            if(iterator==1)
            {
                for(uint i = 0; i<2; i++)
                {
                    cv::line(canvas,
                             warpArray[i][0],
                             warpArray[i][1],
                             regionColor[i],
                             3,
                             cv::LINE_AA);
                }
            }
            else if(iterator==2)
            {
                for(uint i =0; i<2; i++)
                {
                    cv::line(canvas,
                             warpArray[i][1],
                             warpArray[i][2],
                             regionColor[i],
                             3,
                             cv::LINE_AA);
                    cv::line(canvas,
                             warpArray[i][2],
                             warpArray[i][0],
                             regionColor[i],
                             3,
                             cv::LINE_AA);
                }
            }
            Q_EMIT onImageReady(canvas);
            iterator++;
        }
        else
        {
            cancel = false;
        }

    }
    if(iterator == 3)
    {
        image = cv::getAffineTransform(warpArray[0],warpArray[1]);
        mpCVImageData->set_image(image);
    }
}

void
GetAffineTransformModel::
onMouseMoved(QMouseEvent* event, cv::Mat &)
{
    if(holding)
    {
        canvas = save[iterator].clone();
        cv::arrowedLine(canvas,
                        holdingPoint,
                        cv::Point2f(event->x(),event->y()),
                        arrowColor[iterator],
                        3,
                        cv::LINE_AA,
                        0,
                        0.1);
        Q_EMIT onImageReady(canvas);
    }
}


const QString GetAffineTransformModel::mQSWindowName = "Affine Transformation";

const QString GetAffineTransformModel::_category = QString("Image Transformation");

const QString GetAffineTransformModel::_model_name = QString( "Affine Transformation" );
