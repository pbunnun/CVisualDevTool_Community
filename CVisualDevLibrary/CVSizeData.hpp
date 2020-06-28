#ifndef CVSIZEDATA_HPP
#define CVSIZEDATA_HPP

#include <opencv2/core/core.hpp>

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class CVSizeData : public NodeData
{
public:

    CVSizeData()
        : mCVSize()
    {}

    CVSizeData( cv::Size const & size )
        : mCVSize( size )
    {}

    NodeDataType
    type() const override
    {
        return { "size", "Sze" };
    }

    cv::Size &
    size()
    {
        return mCVSize;
    }

private:
    cv::Size mCVSize;

};

#endif // CVSIZEDATA_HPP
