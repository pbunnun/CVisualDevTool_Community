#include "FaceDetectionModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include "qtvariantproperty.h"

using namespace std;
using namespace cv;

CascadeClassifier cascade;
int boxSize;

FaceDetectionModel::FaceDetectionModel() : PBNodeDataModel( _model_name, true ), 
    mpEmbeddedWidget( new FaceDetectionEmbeddedWidget() ),
    _minPixmap( ":FaceDetection.png" ) 
{
    qRegisterMetaType<cv::Mat>( "cv::Mat&" );
    connect( mpEmbeddedWidget, &FaceDetectionEmbeddedWidget::button_clicked_signal, this, &FaceDetectionModel::em_button_clicked );        
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );
    mpSyncData = make_shared< SyncData >();
        
    cascade.load(samples::findFile("haarcascades/haarcascade_frontalface_default.xml"));
        
    EnumPropertyType enumPropertyType;
    enumPropertyType.mslEnumNames = mpEmbeddedWidget->get_combobox_string_list();
    enumPropertyType.miCurrentIndex = 0;
    QString propId = "combobox_id";
    auto propComboBox = std::make_shared< TypedProperty< EnumPropertyType > >("ComboBox", propId, QtVariantPropertyManager::enumTypeId(), enumPropertyType);
    mvProperty.push_back( propComboBox );
    mMapIdToProperty[ propId ] = propComboBox;
}

unsigned int
FaceDetectionModel::nPorts(PortType portType) const {
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
FaceDetectionModel::dataType(PortType, PortIndex portIndex) const {
    if(portIndex==0)
        return CVImageData().type();
    else if(portIndex == 1)
        return SyncData().type();
    return NodeDataType();
}

std::shared_ptr<NodeData>
FaceDetectionModel::outData(PortIndex I) {
    if( isEnable() )
    {
        if(I==0)
            return mpCVImageData;
        else if(I==1)
            return mpSyncData;
    }
    return nullptr;
}

void FaceDetectionModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex) {

    if( !isEnable() )
        return;

    if (nodeData)
    {
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );

        if( d )
        {
            mpCVImageInData = d;
            cv::Mat FaceDetectedImage = processData(mpCVImageInData);
            mpCVImageData = std::make_shared<CVImageData>(FaceDetectedImage);
        }
        mpSyncData->emit();
        Q_EMIT dataUpdated(1);
    }

    Q_EMIT dataUpdated(0);
}

cv::Mat FaceDetectionModel::processData(const std::shared_ptr<CVImageData> &p)
{
    cv::Mat grayScaled, reducedSize, img;
    img = p->image().clone();
    vector<Rect> objects;
    cvtColor(img, grayScaled, COLOR_BGR2GRAY );

    resize(grayScaled, reducedSize, Size(), 1, 1, INTER_LINEAR );
    equalizeHist(reducedSize, reducedSize);

    cascade.detectMultiScale(reducedSize, objects, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    for ( size_t i = 0; i < objects.size(); i++ ) {
        Rect r = objects[i];
        Point topLeft(cvRound(r.x)-boxSize, cvRound(r.y)-boxSize);
        Point bottomRight(cvRound(r.x+r.width)+boxSize, cvRound(r.y+r.height)+boxSize);
        rectangle(img, topLeft, bottomRight, Scalar(255, 0, 0), 8, 8, 0);
    }
    return img;
}

QJsonObject FaceDetectionModel::save() const {

    QJsonObject modelJson = PBNodeDataModel::save();

    QJsonObject cParams;
    cParams[ "combobox_text" ] = mpEmbeddedWidget->get_combobox_text();
    modelJson[ "cParams" ] = cParams;

    return modelJson;
}

void FaceDetectionModel::setModelProperty( QString & id, const QVariant & value ) {
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
    PBNodeDataModel::setModelProperty( id, value );

    if( !mMapIdToProperty.contains( id ) )
        return;

    auto prop = mMapIdToProperty[ id ];
    auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >(mMapIdToProperty[ id ]);
    typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( value.toString() );

    boxSize = 25;
    switch( value.toInt() ) {
        case 0:
            cascade.load(samples::findFile("haarcascades/haarcascade_frontalface_default.xml"));
            break;
        case 1:
            cascade.load(samples::findFile("haarcascades/haarcascade_frontalface_alt2.xml"));
            break;
        case 2:
            cascade.load(samples::findFile("haarcascades/haarcascade_frontalface_alt.xml"));
            break;
        case 3:
            cascade.load(samples::findFile("haarcascades/haarcascade_eye_tree_eyeglasses.xml"));
            boxSize = 5;
            break;
    }
    mpEmbeddedWidget->set_combobox_value( value.toString() );
    if(mpCVImageInData)
    {
        cv::Mat FaceDetectedImage = processData(mpCVImageInData);
        mpCVImageData->set_image(FaceDetectedImage);
    }
    mpSyncData->emit();
    Q_EMIT dataUpdated(1);
}

void FaceDetectionModel::em_button_clicked( int button ) {
    if( button == 3 ) {
        auto prop = mMapIdToProperty[ "combobox_id" ];
        auto typedProp = std::static_pointer_cast< TypedProperty< EnumPropertyType > >( prop );
        typedProp->getData().miCurrentIndex = typedProp->getData().mslEnumNames.indexOf( mpEmbeddedWidget->get_combobox_text() );
        Q_EMIT property_changed_signal( prop );
    }
}

const QString FaceDetectionModel::_category = QString("Image Processing");

const QString FaceDetectionModel::_model_name = QString( "Face Detection" );
