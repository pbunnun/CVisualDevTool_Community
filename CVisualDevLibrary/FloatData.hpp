#ifndef FLOATDATA_HPP
#define FLOATDATA_HPP

#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class FloatData : public NodeData
{
public:
    FloatData()
        : mfData( 0 )
    {}

    FloatData( float data )
        : mfData( data )
    {}

    NodeDataType
    type() const override
    {
        return { "float", "Flt" };
    }

    float &
    number()
    {
        return mfData;
    }

private:
    float mfData;
};

#endif // FLOATDATA_HPP
