#ifndef CVSCALARDATA_H
#define CVSCALARDATA_H

#pragma once

#include <opencv2/core.hpp>

#include <nodes/NodeDataModel>
#include "InformationData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class CVScalarData : public InformationData
{
public:
    CVScalarData()
        : mCVScalar()
    {}

    FloatData( cv::Scalar const & scalar )
        : mCVScalar( scalar )
    {}

    NodeDataType
    type() const override
    {
        return { "information", "Scl" };
    }

    float &
    scalar()
    {
        return mCVScalar;
    }

    void set_information() override
    {
        mQSData = QString("<%1 , %2 , %3>")
                  .arg(mCVScalar[0]).arg(mCVScalar[1]).arg(mCVScalar[2]);
    }

private:
    cv::Scalar mCVScalar;
};

#endif // CVSCALARDATA_H
