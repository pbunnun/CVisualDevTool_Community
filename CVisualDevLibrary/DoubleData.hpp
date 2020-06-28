#ifndef DOUBLEDATA_HPP
#define DOUBLEDATA_HPP

#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class DoubleData : public NodeData
{
public:
    DoubleData()
        : mdData( 0 )
    {}

    DoubleData( double data )
        : mdData( data )
    {}

    NodeDataType
    type() const override
    {
        return { "double", "Dbl" };
    }

    double &
    number()
    {
        return mdData;
    }

private:
    double mdData;
};

#endif // DOUBLEDATA_HPP
