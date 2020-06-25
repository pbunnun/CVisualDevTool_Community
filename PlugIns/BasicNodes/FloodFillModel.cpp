#include "FloodFillModel.hpp"

#include <QDebug> //for debugging using qDebug()

#include <nodes/DataModelRegistry>

#include <opencv2/imgproc.hpp>
#include "qtvariantproperty.h"

FloodFillModel::
FloodFillModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget(new FloodFillEmbeddedWidget),
      _minPixmap( ":FloodFill.png" )
{
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &FloodFillEmbeddedWidget::spinbox_clicked_signal, this, &FloodFillModel::em_spinbox_clicked );

    PointPropertyType pointPropertyType;
    pointPropertyType.miXPosition = mParams.mCVPointSeed.x;
    pointPropertyType.miYPosition = mParams.mCVPointSeed.y;
    QString propId = "seed_point";
    auto propSeedPoint = std::make_shared< TypedProperty< PointPropertyType > >( "Seed Point", propId, QVariant::Point, pointPropertyType, "Operation");
    mvProperty.push_back( propSeedPoint );
    mMapIdToProperty[ propId ] = propSeedPoint;

    UcharPropertyType ucharPropertyType;
    for(int i=0; i<4; i++)
    {
        ucharPropertyType.mucValue = mParams.mucFillColor[i];
        propId = QString("fill_color_%1").arg(i);
        auto propFillColor = std::make_shared< TypedProperty< UcharPropertyType > >( QString::fromStdString("Fill Color "+color[i]), propId, QVariant::Int, ucharPropertyType, "Operation");
        mvProperty.push_back( propFillColor );
        mMapIdToProperty[ propId ] = propFillColor;

        ucharPropertyType.mucValue = mParams.mucLowerDiff[i];
        propId = QString("lower_diff_%1").arg(i);
        auto propLowerDiff = std::make_shared< TypedProperty< UcharPropertyType > >( QString::fromStdString("Lower Diff "+color[i]), propId, QVariant::Int, ucharPropertyType, "Operation");
        mMapIdToProperty[ propId ] = propFillColor;

        ucharPropertyType.mucValue = mParams.mucFillColor[i];
        propId = QString("upper_diff_%1").arg(i);
        auto propUpperDiff = std::make_shared< TypedProperty< UcharPropertyType > >( QString::fromStdString("Upper Diff "+color[i]), propId, QVariant::Int, ucharPropertyType, "Operation");
        mMapIdToProperty[ propId ] = propFillColor;
    }

    mpEmbeddedWidget->get_lowerB_spinbox()->setValue(mParams.mucLowerDiff[0]);
    mpEmbeddedWidget->get_lowerG_spinbox()->setValue(mParams.mucLowerDiff[1]);
    mpEmbeddedWidget->get_lowerR_spinbox()->setValue(mParams.mucLowerDiff[2]);
    mpEmbeddedWidget->get_lowerGray_spinbox()->setValue(mParams.mucLowerDiff[3]);
    mpEmbeddedWidget->get_upperB_spinbox()->setValue(mParams.mucUpperDiff[0]);
    mpEmbeddedWidget->get_upperG_spinbox()->setValue(mParams.mucUpperDiff[1]);
    mpEmbeddedWidget->get_upperR_spinbox()->setValue(mParams.mucUpperDiff[2]);
    mpEmbeddedWidget->get_upperGray_spinbox()->setValue(mParams.mucUpperDiff[3]);

    propId = "define_boundaries";
    auto propDefineBoundaries = std::make_shared< TypedProperty <bool> >("Define Boundaries", propId, QVariant::Bool, mParams.mbDefineBoundaries, "Display");
    mvProperty.push_back( propDefineBoundaries );
    mMapIdToProperty[ propId ] = propDefineBoundaries;

    pointPropertyType.miXPosition = mParams.mCVPointRect1.x;
    pointPropertyType.miYPosition = mParams.mCVPointRect1.y;
    propId = "rect_point_1";
    auto propRectPoint1 = std::make_shared< TypedProperty< PointPropertyType > >( "Boundary Point 1", propId, QVariant::Point, pointPropertyType, "Display");
    mvProperty.push_back( propRectPoint1 );
    mMapIdToProperty[ propId ] = propRectPoint1;

    pointPropertyType.miXPosition = mParams.mCVPointRect2.x;
    pointPropertyType.miYPosition = mParams.mCVPointRect2.y;
    propId = "rect_point_2";
    auto propRectPoint2 = std::make_shared< TypedProperty< PointPropertyType > >( "Boundary Point 2", propId, QVariant::Point, pointPropertyType, "Display");
    mvProperty.push_back( propRectPoint2 );
    mMapIdToProperty[ propId ] = propRectPoint2;

    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = QStringList({"4 neighbor pixels","8 neighbor pixels", "FLOODFILL_MASK_ONLY", "FLOODFILL_FIXED_RANGE"});
    enumPropertyType.miCurrentIndex = 0;
    propId = "flags";
    auto propFlags = std::make_shared< TypedProperty < EnumPropertyType > > ("Flags", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType, "Operation");
    mvProperty.push_back( propFlags );
    mMapIdToProperty[ propId ] = propFlags;
}

unsigned int
FloodFillModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 2;
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
FloodFillModel::
dataType(PortType, PortIndex) const
{
    return CVImageData().type();
}


std::shared_ptr<NodeData>
FloodFillModel::
outData(PortIndex)
{
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void
FloodFillModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast<CVImageData>(nodeData);
        if (d)
        {
            mapCVImageInData[portIndex] = d;
            if(mapCVImageInData[0])
            {
                toggle_widgets();
                processData(mapCVImageInData, mpCVImageData, mParams, mProps);
            }
        }
    }

    Q_EMIT dataUpdated(0);
}

QJsonObject
FloodFillModel::
save() const
{
    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams["seedPointX"] = mParams.mCVPointSeed.x;
    cParams["seedPointY"] = mParams.mCVPointSeed.y;
    for(int i=0; i<4; i++)
    {
        cParams[QString("fillColor%1").arg(i)] = mParams.mucFillColor[i];
        cParams[QString("lowerDiff%1").arg(i)] = mParams.mucLowerDiff[i];
        cParams[QString("upperDiff%1").arg(i)] = mParams.mucUpperDiff[i];
    }
    cParams["defineBoundaries"] = mParams.mbDefineBoundaries;
    cParams["rectPoint1X"] = mParams.mCVPointRect1.x;
    cParams["rectPoint1Y"] = mParams.mCVPointRect1.y;
    cParams["rectPoint2X"] = mParams.mCVPointRect2.x;
    cParams["rectPoint2Y"] = mParams.mCVPointRect2.y;
    cParams["flags"] = mParams.miFlags;
    modelJson["cParams"] = cParams;

    return modelJson;
}

void
FloodFillModel::
restore(QJsonObject const& p)
{
    PBNodeDataModel::restore(p);

    QJsonObject paramsObj = p[ "cParams" ].toObject();
    if( !paramsObj.isEmpty() )
    {
        QJsonValue argX = paramsObj[ "seedPointX" ];
        QJsonValue argY = paramsObj[ "seedPointY" ];
        if( !argX.isUndefined() && !argY.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "seed_point" ];
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = argX.toInt();
            typedProp->getData().miYPosition = argY.toInt();

            mParams.mCVPointSeed = cv::Point(argX.toInt(),argY.toInt());
        }
        QJsonValue v;
        for(int i=0; i<4; i++)
        {
            v = paramsObj[QString("fillColor%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("fill_color_%1").arg(i)];
                auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
                typedProp->getData().mucValue = v.toInt();

                mParams.mucFillColor[i] = v.toInt();
            }
            v = paramsObj[QString("lowerDiff%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("lower_diff_%1").arg(i)];
                auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
                typedProp->getData().mucValue = v.toInt();

                mParams.mucLowerDiff[i] = v.toInt();
                switch(i)
                {
                case 0:
                    mpEmbeddedWidget->get_lowerB_spinbox()->setValue(v.toInt());
                    break;

                case 1:
                    mpEmbeddedWidget->get_lowerG_spinbox()->setValue(v.toInt());
                    break;

                case 2:
                    mpEmbeddedWidget->get_lowerR_spinbox()->setValue(v.toInt());
                    break;

                case 3:
                    mpEmbeddedWidget->get_lowerGray_spinbox()->setValue(v.toInt());
                    break;
                }
            }
            v = paramsObj[QString("UpperDiff%1").arg(i)];
            if( !v.isUndefined() )
            {
                auto prop = mMapIdToProperty[QString("upper_diff_%1").arg(i)];
                auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
                typedProp->getData().mucValue = v.toInt();

                mParams.mucUpperDiff[i] = v.toInt();
                switch(i)
                {
                case 0:
                    mpEmbeddedWidget->get_upperB_spinbox()->setValue(v.toInt());
                    break;

                case 1:
                    mpEmbeddedWidget->get_upperG_spinbox()->setValue(v.toInt());
                    break;

                case 2:
                    mpEmbeddedWidget->get_upperR_spinbox()->setValue(v.toInt());
                    break;

                case 3:
                    mpEmbeddedWidget->get_upperGray_spinbox()->setValue(v.toInt());
                    break;
                }
            }
        }
        v = paramsObj["defineBoundarie"];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "define_boundaries" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < bool > >(prop);
            typedProp->getData() = v.toBool();

            mParams.mbDefineBoundaries = v.toBool();
        }
        argX = paramsObj[ "rectPoint1X" ];
        argY = paramsObj[ "rectPoint1Y" ];
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
        v = paramsObj[ "flags" ];
        if( !v.isUndefined() )
        {
            auto prop = mMapIdToProperty[ "flags" ];
            auto typedProp = std::static_pointer_cast< TypedProperty < EnumPropertyType > >(prop);
            typedProp->getData().miCurrentIndex = v.toInt();

            mParams.miFlags = v.toInt();
        }
    }
}

void
FloodFillModel::
setModelProperty( QString & id, const QVariant & value )
{
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    const int& maxX = mapCVImageInData[0]->image().cols;
    const int& maxY = mapCVImageInData[0]->image().rows;

    auto prop = mMapIdToProperty[ id ];
    if( id == "seed_point" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        QPoint sPoint =  value.toPoint();
        bool adjValue = false;
        if( sPoint.x() > maxX )
        {
            sPoint.setX(maxX);
            adjValue = true;
        }
        else if( sPoint.x() < 0)
        {
            sPoint.setX(0);
            adjValue = true;
        }
        if( sPoint.y() > maxY)
        {
            sPoint.setY(maxY);
            adjValue = true;
        }
        else if( sPoint.y() < 0)
        {
            sPoint.setY(0);
            adjValue = true;
        }
        if( adjValue )
        {
            typedProp->getData().miXPosition = sPoint.x();
            typedProp->getData().miYPosition = sPoint.y();

            Q_EMIT property_changed_signal( prop );
            return;
        }
        else
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
            typedProp->getData().miXPosition = sPoint.x();
            typedProp->getData().miYPosition = sPoint.y();

            mParams.mCVPointSeed = cv::Point( sPoint.x(), sPoint.y() );
        }
    }
    for(int i=0; i<4; i++)
    {
        if( id == QString("fill_color_%1").arg(i) )
        {
            auto typedProp = std::static_pointer_cast< TypedProperty< UcharPropertyType > >( prop );
            typedProp->getData().mucValue = value.toInt();

            mParams.mucFillColor[i] = value.toInt();
        }
    }
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
    else if( id == "rect_point_2" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty< PointPropertyType > >( prop );
        QPoint rPoint2 =  value.toPoint();
        bool adjValue = false;
        if( rPoint2.x() > maxX )
        {
            rPoint2.setX(maxX);
            adjValue = true;
        }
        else if( rPoint2.x() < 0)
        {
            rPoint2.setX(0);
            adjValue = true;
        }
        if( rPoint2.y() > maxY)
        {
            rPoint2.setY(maxY);
            adjValue = true;
        }
        else if( rPoint2.y() < 0)
        {
            rPoint2.setY(0);
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
    else if( id == "flags" )
    {
        auto typedProp = std::static_pointer_cast< TypedProperty < EnumPropertyType > >(prop);
        typedProp->getData().miCurrentIndex = value.toInt();

        switch(value.toInt())
        {
        case 0:
            mParams.miFlags = 4;
            break;

        case 1:
            mParams.miFlags = 8;
            break;

        case 2:
            mParams.miFlags = cv::FLOODFILL_MASK_ONLY;
            break;

        case 3:
            mParams.miFlags = cv::FLOODFILL_FIXED_RANGE;
            break;
        }
    }

    if(mapCVImageInData[0])
    {
        toggle_widgets();
        processData( mapCVImageInData, mpCVImageData, mParams, mProps );
        Q_EMIT dataUpdated(0);
    }
}

void FloodFillModel::em_spinbox_clicked(int spinbox)
{
    switch(spinbox)
    {
    case 0:
        mParams.mucLowerDiff[0] = mpEmbeddedWidget->get_lowerB_spinbox()->value();
        break;

    case 1:
        mParams.mucLowerDiff[1] = mpEmbeddedWidget->get_lowerG_spinbox()->value();
        break;

    case 2:
        mParams.mucLowerDiff[2] = mpEmbeddedWidget->get_lowerR_spinbox()->value();
        break;

    case 3:
        mParams.mucLowerDiff[3] = mpEmbeddedWidget->get_lowerGray_spinbox()->value();
        break;

    case 4:
        mParams.mucUpperDiff[0] = mpEmbeddedWidget->get_upperB_spinbox()->value();
        break;

    case 5:
        mParams.mucUpperDiff[1] = mpEmbeddedWidget->get_upperG_spinbox()->value();
        break;

    case 6:
        mParams.mucUpperDiff[2] = mpEmbeddedWidget->get_upperR_spinbox()->value();
        break;

    case 7:
        mParams.mucUpperDiff[3] = mpEmbeddedWidget->get_upperGray_spinbox()->value();
        break;
    }
    if(mapCVImageInData[0])
    {
        processData(mapCVImageInData,mpCVImageData,mParams,mProps);
        Q_EMIT dataUpdated(0);
    }
}

void
FloodFillModel::
processData(const std::shared_ptr< CVImageData > (&in)[2], std::shared_ptr<CVImageData> & out,
            const FloodFillParameters & params, FloodFillProperties &props)
{
    cv::Mat& in_image = in[0]->image();
    cv::Mat& out_image = out->image();
    out->set_image(in_image);
    props.mbActiveMask = (in[1]!=nullptr && in[1]->image().channels()==1
    &&in[1]->image().cols==in_image.cols+2
    && in[1]->image().rows==in_image.rows+2)? true : false ;
    mpEmbeddedWidget->set_maskStatus_label(props.mbActiveMask);
    if(params.mbDefineBoundaries)
    {
        cv::Rect rect = {params.mCVPointRect1,params.mCVPointRect2};
        switch(in_image.channels())
        {
        case 1:
            if(props.mbActiveMask)
            {
            cv::floodFill(out_image,
                          in[1]->image(),
                          params.mCVPointSeed,
                          cv::Scalar(params.mucFillColor[3]),
                          &rect,
                          cv::Scalar(params.mucLowerDiff[3]),
                          cv::Scalar(params.mucUpperDiff[3]),
                          params.miFlags);
            }
            else
            {
                cv::floodFill(out_image,
                              cv::Mat(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[3]),
                              &rect,
                              cv::Scalar(params.mucLowerDiff[3]),
                              cv::Scalar(params.mucUpperDiff[3]),
                              params.miFlags);
            }
            break;

        case 3:
            if(props.mbActiveMask)
            {
                cv::floodFill(out_image,
                              in[1]->image(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[0],params.mucFillColor[1],params.mucFillColor[2]),
                              &rect,
                              cv::Scalar(params.mucLowerDiff[0],params.mucLowerDiff[1],params.mucLowerDiff[2]),
                              cv::Scalar(params.mucUpperDiff[0],params.mucUpperDiff[1],params.mucUpperDiff[2]),
                              params.miFlags);
            }
            else
            {
                cv::floodFill(out_image,
                              cv::Mat(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[0],params.mucFillColor[1],params.mucFillColor[2]),
                              &rect,
                              cv::Scalar(params.mucLowerDiff[0],params.mucLowerDiff[1],params.mucLowerDiff[2]),
                              cv::Scalar(params.mucUpperDiff[0],params.mucUpperDiff[1],params.mucUpperDiff[2]),
                              params.miFlags);
            }
            break;
        }
    }
    else
    {
        switch(in_image.channels())
        {
        case 1:
            if(props.mbActiveMask)
            {
            cv::floodFill(out_image,
                          in[1]->image(),
                          params.mCVPointSeed,
                          cv::Scalar(params.mucFillColor[3]),
                          0,
                          cv::Scalar(params.mucLowerDiff[3]),
                          cv::Scalar(params.mucUpperDiff[3]),
                          params.miFlags);
            }
            else
            {
                cv::floodFill(out_image,
                              cv::Mat(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[3]),
                              0,
                              cv::Scalar(params.mucLowerDiff[3]),
                              cv::Scalar(params.mucUpperDiff[3]),
                              params.miFlags);
            }
            break;

        case 3:
            if(props.mbActiveMask)
            {
                cv::floodFill(out_image,
                              in[1]->image(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[0],params.mucFillColor[1],params.mucFillColor[2]),
                              0,
                              cv::Scalar(params.mucLowerDiff[0],params.mucLowerDiff[1],params.mucLowerDiff[2]),
                              cv::Scalar(params.mucUpperDiff[0],params.mucUpperDiff[1],params.mucUpperDiff[2]),
                              params.miFlags);
            }
            else
            {
                cv::floodFill(out_image,
                              cv::Mat(),
                              params.mCVPointSeed,
                              cv::Scalar(params.mucFillColor[0],params.mucFillColor[1],params.mucFillColor[2]),
                              0,
                              cv::Scalar(params.mucLowerDiff[0],params.mucLowerDiff[1],params.mucLowerDiff[2]),
                              cv::Scalar(params.mucUpperDiff[0],params.mucUpperDiff[1],params.mucUpperDiff[2]),
                              params.miFlags);
            }
            break;
        }
    }
}

bool FloodFillModel::allports_are_active(const std::shared_ptr<CVImageData> (&ap)[2]) const
{
    for(const std::shared_ptr<CVImageData> &p : ap)
    {
        if(p==nullptr)
        {
            return false;
        }
    }
    return true;
}

void FloodFillModel::toggle_widgets() const
{
    switch(mapCVImageInData[0]->image().channels())
    {
    case 1:

        mpEmbeddedWidget->enable_lowerB_label(false);
        mpEmbeddedWidget->enable_lowerG_label(false);
        mpEmbeddedWidget->enable_lowerR_label(false);
        mpEmbeddedWidget->enable_upperG_label(false);
        mpEmbeddedWidget->enable_upperB_label(false);
        mpEmbeddedWidget->enable_upperG_label(false);

        mpEmbeddedWidget->get_lowerB_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_lowerG_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_lowerR_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_upperB_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_upperG_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_upperR_spinbox()->setEnabled(false);

        mpEmbeddedWidget->enable_lowerGray_label(true);
        mpEmbeddedWidget->enable_upperGray_label(true);

        mpEmbeddedWidget->get_lowerGray_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_upperGray_spinbox()->setEnabled(true);

        break;

    case 3:

        mpEmbeddedWidget->enable_lowerGray_label(false);
        mpEmbeddedWidget->enable_upperGray_label(false);

        mpEmbeddedWidget->get_lowerGray_spinbox()->setEnabled(false);
        mpEmbeddedWidget->get_upperGray_spinbox()->setEnabled(false);

        mpEmbeddedWidget->enable_lowerB_label(true);
        mpEmbeddedWidget->enable_lowerG_label(true);
        mpEmbeddedWidget->enable_lowerR_label(true);
        mpEmbeddedWidget->enable_upperG_label(true);
        mpEmbeddedWidget->enable_upperB_label(true);
        mpEmbeddedWidget->enable_upperG_label(true);

        mpEmbeddedWidget->get_lowerB_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_lowerG_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_lowerR_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_upperB_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_upperG_spinbox()->setEnabled(true);
        mpEmbeddedWidget->get_upperR_spinbox()->setEnabled(true);

        break;
    }
}

const QString FloodFillModel::_category = QString( "Image Operation" );

const QString FloodFillModel::_model_name = QString( "Flood Fill" );

const std::string FloodFillModel::color[4] = {"B","G","R","Gray"};
