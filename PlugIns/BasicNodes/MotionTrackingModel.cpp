#include "MotionTrackingModel.hpp"

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int x = 0;
Mat fgMaskMOG;
Ptr<BackgroundSubtractor> pMOG;
Rect nullRect(-1, -1, 0, 0);
vector<pair<pair<int, int>, Rect>> history; //{{objectID, timer}, object}
vector<pair<pair<int, int>, pair<Point, Point>>> lines; //{{lineID, timer}, line}

MotionTrackingModel::MotionTrackingModel(): PBNodeDataModel( _model_name, true ),
      _minPixmap( ":MotionTracking.png" ) {

    pMOG = createBackgroundSubtractorMOG2(720, 64, false);
    mpCVImageData = std::make_shared< CVImageData >( cv::Mat() );

    history.push_back({{0, 0}, nullRect});
}

unsigned int MotionTrackingModel::nPorts(PortType portType) const {
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

NodeDataType MotionTrackingModel::dataType(PortType, PortIndex) const {
    return CVImageData().type();
}

std::shared_ptr<NodeData> MotionTrackingModel::outData(PortIndex) {
    if( isEnable() )
        return mpCVImageData;
    else
        return nullptr;
}

void MotionTrackingModel::setInData( std::shared_ptr< NodeData > nodeData, PortIndex ) {
    if(!isEnable())
        return;

    if( nodeData )
    {
        auto d = std::dynamic_pointer_cast< CVImageData >(nodeData);
        if( d )
        {
            cv::Mat MotionTrackedImage = processData(d);
            mpCVImageData = std::make_shared<CVImageData>(MotionTrackedImage);
        }
    }

    Q_EMIT dataUpdated( 0 );
}

cv::Mat MotionTrackingModel::processData(const std::shared_ptr<CVImageData> &p)
{
    Mat img = p->image().clone();
    pMOG->apply(img, fgMaskMOG);
    std::vector<std::vector<cv::Point>> contours;
    cv::dilate(fgMaskMOG,fgMaskMOG,cv::Mat());
    cv::findContours(fgMaskMOG,contours, RETR_EXTERNAL , CHAIN_APPROX_TC89_L1 );
    cvtColor(fgMaskMOG, fgMaskMOG, COLOR_GRAY2RGB);

    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size()), availableRect, shownRect;

    for(size_t i = 0; i < contours.size(); i++) {
        approxPolyDP(contours[i], contours_poly[i], 36, false);
        boundRect[i] = boundingRect(contours_poly[i]);
        if ((boundRect[i].br().x-boundRect[i].tl().x)*(boundRect[i].br().y-boundRect[i].tl().y) >= 1440) availableRect.push_back(boundRect[i]);
    }

    int thres = 30;
    for(size_t i = 0; i < availableRect.size(); i++) {
        for(size_t f = 0; f < availableRect.size(); f++) {
            cv::Point iCenter(availableRect[i].x+(availableRect[i].width/2), availableRect[i].y+(availableRect[i].height/2));
            cv::Point fCenter(availableRect[f].x+(availableRect[f].width/2), availableRect[f].y+(availableRect[f].height/2));
            if (abs(iCenter.x-fCenter.x) <= (availableRect[i].width/2)+(availableRect[f].width/2)+thres &&
                    abs(iCenter.y-fCenter.y) <= (availableRect[i].height/2)+(availableRect[f].height/2)+thres) {
                availableRect[i] = availableRect[i] & availableRect[f];
                availableRect[f] = availableRect[i] & availableRect[f];
            }
        }
    }

    for(size_t i = 0; i < availableRect.size(); i++) {
        bool repeat = false;
        for(size_t f = i+1; f < availableRect.size(); f++) {
            if (availableRect[i] == availableRect[f] || shownRect.size() >= 8) {
                repeat = true;
                break;
            }
        }
        if (!repeat) shownRect.push_back(availableRect[i]);
    }

    updateHistory(shownRect);

    for(size_t i = 0; i < history.size(); i++) rectangle(img, history[i].second.tl(), history[i].second.br(), getColor(history[i].first.first), 16);
    for(size_t i = 0; i < lines.size(); i++) line(img, lines[i].second.first, lines[i].second.second, getColor(lines[i].first.first), 16);

    return img;
}

void MotionTrackingModel::updateHistory(std::vector<cv::Rect> newRect) {
    for(size_t i = 0; i < history.size(); i++) {
        if (history[i].first.second >= 12) history.erase(history.begin() + i);
        if (history.size() == 0) history.push_back({{0, 0}, nullRect});
    }

    for(size_t i = 0; i < lines.size(); i++) {
        if (lines[i].first.second >= 50) lines.erase(lines.begin() + i);
    }

    for(size_t i = 0; i < newRect.size(); i++) {
        bool matched = false;
        bool free = false;
        int freeIndex = 0;
        size_t historySize = history.size();
        for(size_t f = 0; f < historySize; f++) {
            if ((newRect[i] & history[f].second).area() > 0 && history[f].first.second != -1) {
                lines.push_back({{history[f].first.first, 0},
                                 {Point(history[f].second.x + (history[f].second.width/2), history[f].second.y + (history[f].second.height/2)),
                                Point(newRect[i].x + (newRect[i].width/2), newRect[i].y  + (newRect[i].height/2))}});
                history[f].second = newRect[i];
                history[f].first.second = -1;
                matched = true;
                break;
            }
        }

        if (!matched) {
            while (!free) {
                for (size_t i = 0; i < history.size(); i++) {
                    if (history[i].first.first == freeIndex) {
                        freeIndex++;
                        i = 0;
                        break;
                    } else if (i == history.size() - 1) {
                        free = true;
                    }
                }
            }
            history.push_back({{freeIndex, -1}, newRect[i]});
        }
    }

    for(size_t i = 0; i < history.size(); i++) history[i].first.second++;
    for(size_t i = 0; i < lines.size(); i++) lines[i].first.second++;
}

cv::Scalar MotionTrackingModel::getColor(int placement) {
    cv::Scalar color;
    switch(placement % 16) {
        case 0: return cv::Scalar(0, 0, 255);
        case 1: return cv::Scalar(0, 255, 0);
        case 2: return cv::Scalar(255, 0, 0);
        case 3: return cv::Scalar(0, 255, 255);
        case 4: return cv::Scalar(255, 0, 255);
        case 5: return cv::Scalar(255, 255, 0);
        case 6: return cv::Scalar(255, 255, 255);
        case 7: return cv::Scalar(0, 0, 0);
        case 8: return cv::Scalar(0, 0, 127);
        case 9: return cv::Scalar(0, 127, 0);
        case 10: return cv::Scalar(127, 0, 0);
        case 11: return cv::Scalar(0, 127, 127);
        case 12: return cv::Scalar(127, 0, 127);
        case 13: return cv::Scalar(127, 127, 0);
        case 14: return cv::Scalar(127, 127, 127);
        case 15: return cv::Scalar(127, 0, 255);
        default: return cv::Scalar(127, 255, 0);
    }
}


const QString MotionTrackingModel::_category = QString("Image Operation");

const QString MotionTrackingModel::_model_name = QString( "Motion Tracker" );

