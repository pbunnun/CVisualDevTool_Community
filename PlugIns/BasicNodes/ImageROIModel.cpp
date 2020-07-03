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
      mpEmbeddedWidget( new ImageROIEmbeddedWidget() ),
      _minPixmap(":ImageROI.png")
{
    for(std::shared_ptr<CVImageData>& mp : mapCVImageData)
    {
        mp = std::make_shared<CVImageData>(cv::Mat());
    }
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &ImageROIEmbeddedWidget::button_clicked_signal, this, &ImageROIModel::em_button_clicked );

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

    propId = "display_lines";
    auto propDisplayLines = std::make_shared<TypedProperty<bool>>("Display Lines", propId, QVariant::Bool, mParams.mbDisplayLines, "Display");
    mvProperty.push_back( propDisplayLines );
    mMapIdToProperty[ propId ] = propDisplayLines;

    propId = "lock_output_roi";
    auto propLockOutputROI = std::make_shared<TypedProperty<bool>>("Lock Output ROI", propId, QVariant::Bool, mParams.mbLockOutputROI, "Operation");
    mvProperty.push_back( propLockOutputROI );
    mMapIdToProperty[ propId ] = propLockOutputROI;
}

unsigned int
ImageROIModel::
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
ImageROIModel::
dataType( PortType, PortIndex ) const
{
    return CVImageData().type();
}

std::shared_ptr<NodeData>
ImageROIModel::
outData(PortIndex I)
{
    if( isEnable() )
        return mapCVImageData[I];
    else
        return nullptr;
}

void
ImageROIModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex portIndex)
{
    if( !isEnable() )
        return;

    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mapCVImageInData[portIndex] = d;
            mProps.mbNewMat = true;
            if(mapCVImageInData[0] && !mapCVImageInData[1])
            {
                overwrite( mapCVImageInData[0], mParams);
                processData( mapCVImageInData, mapCVImageData, mParams, mProps);
                mParams.mbLockOutputROI?
                Q_EMIT dataUpdated( 1 ) : updateAllOutputPorts();
            }
            else if(mapCVImageInData[0] && mapCVImageInData[1])
            {
                overwrite( mapCVImageInData[0], mParams);
                processData( mapCVImageInData, mapCVImageData, mParams, mProps);
                Q_EMIT dataUpdated(1);
            }
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
    cParams["displayLines"] = mParams.mbDisplayLines;
    cParams["lockOutputROI"] =mParams.mbLockOutputROI;
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
        v = paramsObj[ "displayLines" ];
        if(! v.isUndefined() )
        {
            auto prop = mMapIdToProperty["display_lines"];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbDisplayLines = v.toBool();
        }
        v = paramsObj[ "lockOutputROIisplayLines" ];
        if(! v.isUndefined() )
        {
            auto prop = mMapIdToProperty["lock_output_roi"];
            auto typedProp = std::static_pointer_cast<TypedProperty<bool>>(prop);
            typedProp->getData() = v.toBool();

            mParams.mbLockOutputROI = v.toBool();
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

    int minX = 0;
    int minY = 0;
    int maxX = mParams.mCVPointRect2.x;
    int maxY = mParams.mCVPointRect2.y;

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
        else if( rPoint1.x() < minX)
        {
            rPoint1.setX(minX);
            adjValue = true;
        }
        if( rPoint1.y() > maxY)
        {
            rPoint1.setY(maxY);
            adjValue = true;
        }
        else if( rPoint1.y() < minY)
        {
            rPoint1.setY(minY);
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

    minX = mParams.mCVPointRect1.x;
    minY = mParams.mCVPointRect1.y;
    maxX = mapCVImageInData[0]->image().cols;
    maxY = mapCVImageInData[0]->image().rows;

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
    else if( id == "display_lines" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbDisplayLines = value.toBool();
    }
    else if( id == "lock_output_roi" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< bool > >( prop );
        typedProp->getData() = value.toBool();

        mParams.mbLockOutputROI = value.toBool();
    }

    if(mapCVImageInData[0] && !mapCVImageInData[1])
    {
        processData( mapCVImageInData, mapCVImageData, mParams, mProps);
        mParams.mbLockOutputROI?
        Q_EMIT dataUpdated( 1 ) : updateAllOutputPorts();
    }
    else if(mapCVImageInData[0] && mapCVImageInData[1])
    {
        processData( mapCVImageInData, mapCVImageData, mParams, mProps);
        mParams.mbLockOutputROI?
        Q_EMIT dataUpdated( 1 ) : updateAllOutputPorts();
    }
}

void ImageROIModel::em_button_clicked( int button )
{
    if(button == 0) //RESET
    {
        mProps.mbReset = true;
        processData( mapCVImageInData, mapCVImageData, mParams, mProps);
        mParams.mbLockOutputROI?
        Q_EMIT dataUpdated( 1 ) : updateAllOutputPorts();
    }
    else if(button ==1) //APPLY
    {
        mProps.mbApply = true;
        processData( mapCVImageInData, mapCVImageData, mParams, mProps);
        Q_EMIT dataUpdated( 1 );
    }
}

void
ImageROIModel::
processData(const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr<CVImageData> (&out)[2],
            const ImageROIParameters &params, ImageROIProperties &props )
{
    mpEmbeddedWidget->enable_reset_button(!in[0]->image().empty());
    const cv::Rect rect(params.mCVPointRect1,params.mCVPointRect2);
    const cv::Mat& in_image = in[0]->image();
    cv::Mat& out_image = out[1]->image();
    out[1]->set_image(in_image);
    static cv::Mat save;
    if(props.mbReset || save.empty() ||props.mbNewMat)
    {
        save = in_image.clone();
    }
    out[1]->set_image(save);
    const bool validROI = in[1]!=nullptr &&
                          !in[1]->image().empty() &&
                          in[1]->image().cols==rect.width &&
                          in[1]->image().rows==rect.height &&
                          in[1]->image().channels() == out_image.channels();

    mpEmbeddedWidget->enable_apply_button(validROI);
    if(props.mbApply)
    {
        cv::Mat roi(out_image,rect);
        cv::addWeighted(in[1]->image(),1,roi,0,0,roi);
    }
    save = out_image.clone();
    out[0]->set_image(cv::Mat(in_image,rect));

    const int& d_rows = out_image.rows;
    const int& d_cols = out_image.cols;
    const cv::Scalar color(params.mucLineColor[0],params.mucLineColor[1],params.mucLineColor[2]);
    if(params.mbDisplayLines)
    {
        if(out_image.channels()==1)
        {
            cv::cvtColor(out_image,out_image,cv::COLOR_GRAY2BGR);
        }
        cv::line(out_image,
                 cv::Point(params.mCVPointRect1.x,0),
                 cv::Point(params.mCVPointRect1.x,d_rows),
                 color,
                 params.miLineThickness,
                 cv::LINE_8);
        cv::line(out_image,
                 cv::Point(0,params.mCVPointRect1.y),
                 cv::Point(d_cols,params.mCVPointRect1.y),
                 color,
                 params.miLineThickness,
                 cv::LINE_8);
        cv::line(out_image,
                 cv::Point(params.mCVPointRect2.x,0),
                 cv::Point(params.mCVPointRect2.x,d_rows),
                 color,
                 params.miLineThickness,
                 cv::LINE_8);
        cv::line(out_image,
                 cv::Point(0,params.mCVPointRect2.y),
                 cv::Point(d_cols,params.mCVPointRect2.y),
                 color,
                 params.miLineThickness,
                 cv::LINE_8);
    }

    props.mbReset = false;
    props.mbApply = false;
    props.mbNewMat = false;
}

void ImageROIModel::overwrite(const std::shared_ptr<CVImageData> &in, ImageROIParameters &params)
{
    int& row = in->image().rows;
    int& col = in->image().cols;
    if(params.mCVPointRect2.x > col)
    {
        auto prop = mMapIdToProperty["rect_point_2"];
        auto typedProp = std::static_pointer_cast<TypedProperty<PointPropertyType>>(prop);
        typedProp->getData().miXPosition = col;
        params.mCVPointRect2.x = col;
    }
    if(params.mCVPointRect2.y > row)
    {
        auto prop = mMapIdToProperty["rect_point_2"];
        auto typedProp = std::static_pointer_cast<TypedProperty<PointPropertyType>>(prop);
        typedProp->getData().miYPosition = row;
        params.mCVPointRect2.y = row;
    }
}

const std::string ImageROIModel::color[3] = {"B", "G", "R"};

const QString ImageROIModel::_category = QString( "Image Operation" );

const QString ImageROIModel::_model_name = QString( "Image ROI" );
