#pragma once

#include <opencv2/core/core.hpp>

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class CVImageData : public NodeData
{
public:

    CVImageData()
        : mCVImage()
    {}

    CVImageData(cv::Mat const &image)
        : mCVImage(image)
    {}

    NodeDataType
    type() const override
    {
        //       id      name
        return {"image", "Mat"};
    }

    void
    set_image (const cv::Mat image ) { mCVImage = image; }

    cv::Mat
    image() const { return mCVImage; }

private:

    cv::Mat mCVImage;
};
