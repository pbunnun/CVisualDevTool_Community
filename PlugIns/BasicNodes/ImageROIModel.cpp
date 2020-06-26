#include "ImageROIModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>
#include <QDebug>
#include <opencv2/imgproc.hpp>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

ImageROIModel::
ImageROIModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new PBImageDisplayWidget() ),
      _minPixmap(":ImageROI.png")
{
    mpEmbeddedWidget->installEventFilter( this );
    mpCVImageData = std::make_shared<CVImageData>(cv::Mat());

    PointPropertyType pointPropertyType;
    pointPropertyType.miXPosition = mParams.mCVPointRect1.x;
    pointPropertyType.miYPosition = mParams.mCVPointRect1.y;
    QString propId = "rect_point_1";
    auto propRectPoint1 = std::make_shared< TypedProperty< PointPropertyType > >("Point 1", propId, QVariant::Point, pointPropertyType, "Operation");
    mvProperty.push_back( propRectPoint1 );
    mMapIdToProperty[ propId ] = propRectPoint1;

    pointPropertyType.miXPosition = mParams.mCVPointRect2.x;
    pointPropertyType.miYPosition = mParams.mCVPointRect2.y;
    propId = "rect_point_2";
    auto propRectPoint2 = std::make_shared< TypedProperty< PointPropertyType > >("Point 2", propId, QVariant::Point, pointPropertyType, "Operation");
    mvProperty.push_back( propRectPoint2 );
    mMapIdToProperty[ propId ] = propRectPoint2;

    UcharPropertyType ucharPropertyType;
    for(int i=0; i<3; i++)
    {
        ucharPropertyType.mucValue = mParams.mucLineColor[i];
        propId = QString("line_color_%1").arg(i);
        auto propLineColor = std::make_shared< TypedProperty< UcharPropertyType > >( QString::fromStdString("Line Color "+color[i]), propId, QVariant::Int, ucharPropertyType, "Display");
        mvProperty.push_back( propLineColor );
        mMapIdToProperty[ propId ] = propLineColor;
    }

    IntPropertyType intPropertyType;
    intPropertyType.miValue = mParams.miLineThickness;
    propId = "line_thickness";
    auto propLineThickness = std::make_shared<TypedProperty<IntPropertyType>>("Line Thickness", propId, QVariant::Int, intPropertyType, "Display");
    mvProperty.push_back( propLineThickness );
    mMapIdToProperty[ propId ] = propLineThickness;
}

unsigned int
ImageROIModel::
nPorts(PortType portType) const
{
    if( portType == PortType::In )
        return 1;
    else
        return 1;
}

bool
ImageROIModel::
eventFilter(QObject *object, QEvent *event)
{
    if( object == mpEmbeddedWidget )
    {
        if( event->type() == QEvent::Resize )
            display_image();
    }
    return false;
}


NodeDataType
ImageROIModel::
dataType( PortType, PortIndex ) const
{
    return CVImageData().type();
}

std::shared_ptr<NodeData>
ImageROIModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
ImageROIModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mpCVImageInData = d;
            processData( mpCVImageInData, mpCVImageData, mParams);
        }
    }
}

QJsonObject
ImageROIModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["rectPoint1X"] = mParams.mCVPointRect1.x;
    cParams["rectPoint1Y"] = mParams.mCVPointRect1.y;
    cParams["rectPoint2X"] = mParams.mCVPointRect2.x;
    cParams["rectPoint2Y"] = mParams.mCVPointRect2.y;
    for(int i=0; i<3; i++)
    {
        cParams[QString("lineColor%1").arg(i)] = mParams.mucLineColor[i];
    }
    cParams["lineThickness"] = mParams.miLineThickness;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
ImageROIModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue argX = paramsObj[ "rectPoint1X" ];
        QJsonValue argY = paramsObj[ "rectPoint1Y" ];
        if( !argX.isUndefined() && !argY.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "rect_point_1" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = argX.toInt();
            typedProp->getData().miYPosition = argY.toInt();

            mParams.mCVPointRect1 = cv::Point(argX.toInt(),argY.toInt());
        }
        argX = paramsObj[ "rectPoint2X" ];
        argY = paramsObj[ "rectPoint2Y" ];
        if( !argX.isUndefined() && !argY.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "rect_point_2" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = argX.toInt();
            typedProp->getData().miYPosition = argY.toInt();

            mParams.mCVPointRect2 = cv::Point(argX.toInt(),argY.toInt());
        }
        QJsonValue v;
        for(int i=0; i<3; i++)
        {
            v = paramsObj[QString("lineColor%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("line_color_%1").arg(i)];
                auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
                typedProp->getData().mucValue = v.toInt();

                mParams.mucLineColor[i] = v.toInt();
            }
        }
        v = paramsObj[ "lineThickness" ];
        if(! v.isUndefined() )
        {
            auto prop = mMapIdToProperty["line_thickness"];
            auto typedProp = std::static_pointer_cast<TypedProperty<IntPropertyType>>(prop);
            typedProp->getData().miValue = v.toInt();

            mParams.miLineThickness = v.toInt();
        }
    }
}

void
ImageROIModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    const int& maxX = mpCVImageInData->image().cols;
    const int& maxY = mpCVImageInData->image().rows;

    auto prop = mMapIdToProperty[ id ];
    if( id == "rect_point_1" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        QPoint rPoint1 =  value.toPoint();
        bool adjValue = false;
        if( rPoint1.x() > maxX )
        {
            rPoint1.setX(maxX);
            adjValue = true;
        }
        else if( rPoint1.x() < 0)
        {
            rPoint1.setX(0);
            adjValue = true;
        }
        if( rPoint1.y() > maxY)
        {
            rPoint1.setY(maxY);
            adjValue = true;
        }
        else if( rPoint1.y() < 0)
        {
            rPoint1.setY(0);
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miXPosition = rPoint1.x();
            typedProp->getData().miYPosition = rPoint1.y();

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = rPoint1.x();
            typedProp->getData().miYPosition = rPoint1.y();

            mParams.mCVPointRect1 = cv::Point( rPoint1.x(), rPoint1.y() );
        }
    }

    const int& minX = mParams.mCVPointRect1.x;
    const int& minY = mParams.mCVPointRect1.y;

    if( id == "rect_point_2" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        QPoint rPoint2 =  value.toPoint();
        bool adjValue = false;
        if( rPoint2.x() > maxX )
        {
            rPoint2.setX(maxX);
            adjValue = true;
        }
        else if( rPoint2.x() < minX)
        {
            rPoint2.setX(minX);
            adjValue = true;
        }
        if( rPoint2.y() > maxY)
        {
            rPoint2.setY(maxY);
            adjValue = true;
        }
        else if( rPoint2.y() < minY)
        {
            rPoint2.setY(minY);
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miXPosition = rPoint2.x();
            typedProp->getData().miYPosition = rPoint2.y();

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = rPoint2.x();
            typedProp->getData().miYPosition = rPoint2.y();

            mParams.mCVPointRect2 = cv::Point( rPoint2.x(), rPoint2.y() );
        }
    }
    for(int i=0; i<3; i++)
    {
        if( id == QString("line_color_%1").arg(i) )
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
            typedProp->getData().mucValue = value.toInt();

            mParams.mucLineColor[i] = value.toInt();
        }
    }
    if( id == "line_thickness" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< IntPropertyType > >( prop );
        typedProp->getData().miValue = value.toInt();

        mParams.miLineThickness = value.toInt();
    }
    if(mpCVImageInData)
    {
        processData(mpCVImageInData,mpCVImageData,mParams);
        Q_EMIT dataUpdated(0);
    }
}

void
ImageROIModel::
display_image()
{
    auto d = std::dynamic_pointer_cast< CVImageData >( mpNodeData );
    if ( d )
        mpEmbeddedWidget->Display( d->image() );
}

void
ImageROIModel::
processData(const std::shared_ptr< CVImageData > &in, std::shared_ptr<CVImageData> &out, const ImageROIParameters &params )
{
    std::shared_ptr<CVImageData> pDisplay = std::make_shared<CVImageData>(cv::Mat());
    cv::Mat& in_image = in->image();
    cv::Mat& display = pDisplay->image();
    if(in_image.channels()==1)
    {
        cv::cvtColor(in_image,display,cv::COLOR_GRAY2BGR);
    }
    else
    {
        pDisplay->set_image(in_image);
    }
    const int& d_rows = display.rows;
    const int& d_cols = display.cols;
    const cv::Scalar color(params.mucLineColor[0],params.mucLineColor[1],params.mucLineColor[2]);
    cv::line(display,
             cv::Point(params.mCVPointRect1.x,0),
             cv::Point(params.mCVPointRect1.x,d_rows),
             color,
             params.miLineThickness,
             cv::LINE_8);
    cv::line(display,
             cv::Point(0,params.mCVPointRect1.y),
             cv::Point(d_cols,params.mCVPointRect1.y),
             color,
             params.miLineThickness,
             cv::LINE_8);
    cv::line(display,
             cv::Point(params.mCVPointRect2.x,0),
             cv::Point(params.mCVPointRect2.x,d_rows),
             color,
             params.miLineThickness,
             cv::LINE_8);
    cv::line(display,
             cv::Point(0,params.mCVPointRect2.y),
             cv::Point(d_cols,params.mCVPointRect2.y),
             color,
             params.miLineThickness,
             cv::LINE_8);

    mpNodeData = std::dynamic_pointer_cast<QtNodes::NodeData>(pDisplay);
    display_image();

    out->set_image(cv::Mat(in_image,cv::Rect(params.mCVPointRect1,params.mCVPointRect2)));
}

const std::string ImageROIModel::color[3] = {"B", "G", "R"};

const QString ImageROIModel::_category = QString( "Image Operation" );

const QString ImageROIModel::_model_name = QString( "Image ROI" );
