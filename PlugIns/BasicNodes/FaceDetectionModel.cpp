#include "FaceDetectionModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

CascadeClassifier cascade;
double scale=1;

FaceDetectionModel::FaceDetectionModel() : PBNodeDataModel( _model_name, true ),_minPixmap( ":FaceDetection.png" ) {
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    cascade.load(samples::findFile("data/haarcascades/haarcascade_frontalface_default.xml")); //faster processing, lower accuracy

    //cascade.load(samples::findFile("data/haarcascades/haarcascade_frontalface_alt.xml")); //slower processing, higher accuracy
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
        result = 1;
        break;

    default:
        break;
    }

    return result;
}

NodeDataType
FaceDetectionModel::dataType(PortType, PortIndex) const {
    return CVImageData().type();
}

std::shared_ptr<NodeData>
FaceDetectionModel::outData(PortIndex) {
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void FaceDetectionModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex) {

    if( !isEnable() )
        return;

    if (nodeData)
    {
        auto d = std::dynamic_pointer_cast< CVImageData >( nodeData );

        if( d )
        {
            cv::Mat FaceDetectedImage = processData(d);
            mpCVImageData = std::make_shared<CVImageData>(FaceDetectedImage);
        }
    }

    Q_EMIT dataUpdated(0);
}

cv::Mat FaceDetectionModel::processData(const std::shared_ptr<CVImageData> &p)
{
    cv::Mat grayScaled, reducedSize, img;
    img = p->image().clone();
    vector<Rect> faces;
    cvtColor(img, grayScaled, COLOR_BGR2GRAY );

    resize(grayScaled, reducedSize, Size(), 1 / scale, 1 / scale, INTER_LINEAR );
    equalizeHist(reducedSize, reducedSize);

    cascade.detectMultiScale(reducedSize, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    for ( size_t i = 0; i < faces.size(); i++ ) {
        Rect r = faces[i];
        Point topLeft(cvRound(r.x*scale)-25, cvRound(r.y*scale)-25);
        Point bottomRight(cvRound((r.x+r.width)*scale)+25, cvRound((r.y+r.height)*scale)+25);
        rectangle(img, topLeft, bottomRight, Scalar(255, 0, 0), 8, 8, 0);
    }
    return img;
}

const QString FaceDetectionModel::_category = QString("Image Operation");

const QString FaceDetectionModel::_model_name = QString( "Face Detector" );
