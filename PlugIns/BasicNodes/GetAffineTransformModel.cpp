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
            processData( mpCVImageInData, mpCVImageData );
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated( 0 );
}

void
GetAffineTransformModel::
onMouseEvents(int event, int x, int y, int, void *pInOut)
{
    cv::Mat& image = (*(onMouseEventsParameters*)pInOut).mCVImage;
    static unsigned int iterator = 0;
    static cv::Mat canvas = image.clone();
    static cv::Mat save[4] = {image};
    static cv::Point2f holdingPoint = cv::Point();
    static cv::Point2f warpArray[2][3] = {{},{}};

    static const cv::Scalar arrowColor[3] = {cv::Scalar(255,0,0),
                                             cv::Scalar(0,255,0),
                                             cv::Scalar(0,0,255)};
    static const cv::Scalar regionColor[2] = {cv::Scalar(255,255,0),
                                              cv::Scalar(0,255,255)};

    static bool cancel = false;
    static bool holding = false;

    if((*(onMouseEventsParameters*)pInOut).mbNewImage)
    {
        (*(onMouseEventsParameters*)pInOut).mbNewImage = false;
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

    if(iterator == 3 && event!=cv::EVENT_RBUTTONDOWN)
    {
        return;
    }

    if(event == cv::EVENT_LBUTTONDOWN)
    {
        holdingPoint = cv::Point2f(x,y);
        holding = true;
    }
    else if(event == cv::EVENT_LBUTTONUP)
    {
        holding = false;
        warpArray[1][iterator] = cv::Point2f(x,y);
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
            cv::imshow(msWindowName,canvas);
            iterator++;
        }
        else
        {
            cancel = false;
        }

    }
    else if(event == cv::EVENT_RBUTTONDOWN)
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
        cv::imshow(msWindowName,canvas);
    }
    else if(event == cv::EVENT_MOUSEMOVE)
    {
        if(holding)
        {
            canvas = save[iterator].clone();
            cv::arrowedLine(canvas,
                            holdingPoint,
                            cv::Point2f(x,y),
                            arrowColor[iterator],
                            3,
                            cv::LINE_AA,
                            0,
                            0.1);
            cv::imshow(msWindowName,canvas);
        }
    }

    if(iterator == 3)
    {
        image = cv::getAffineTransform(warpArray[0],warpArray[1]);
    }
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
processData(const std::shared_ptr< CVImageData > & in, std::shared_ptr< CVImageData > & out )
{
    cv::Mat& in_image = in->image();
    mpEmbeddedWidget->enable_recreate_button(!in_image.empty());
    if(in_image.empty())
    {
        return;
    }
    onMouseEventsParameters inOut(in_image); //automatically assigns mbNewImage to true
    cv::namedWindow(msWindowName,cv::WINDOW_GUI_NORMAL);
    cv::setMouseCallback(msWindowName,onMouseEvents,(void*)&inOut);
    cv::imshow(msWindowName,in_image);
    cv::waitKey(0);
    cv::destroyWindow(msWindowName);
    out->set_image(inOut.mCVImage);
}

const std::string GetAffineTransformModel::msWindowName = "OpenCV Built-in GUI";

const QString GetAffineTransformModel::_category = QString("Image Transformation");

const QString GetAffineTransformModel::_model_name = QString( "Affine Transformation" );
