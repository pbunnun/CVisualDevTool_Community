#ifndef CVRECTDATA_HPP
#define CVRECTDATA_HPP

#include <opencv2/core/core.hpp>

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class CVRectData : public NodeData
{
public:

    CVRectData()
        : mCVRect()
    {}

    CVRectData( cv::Rect const & rect )
        : mCVRect( rect )
    {}

    NodeDataType
    type() const override
    {
        return { "rect", "Rct" };
    }

    cv::Rect &
    rect()
    {
        return mCVRect;
    }

private:
    cv::Rect mCVRect;

};

#endif // CVRECTDATA_HPP
