#ifndef INTEGERDATA_HPP
#define INTEGERDATA_HPP

#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class IntegerData : public NodeData
{
public:
    IntegerData()
        : miData( 0 )
    {}

    IntegerData( int data )
        : miData( data )
    {}

    NodeDataType
    type() const override
    {
        return { "int", "Int" };
    }

    int &
    number()
    {
        return miData;
    }

private:
    int miData;
};

#endif // INTEGERDATA_HPP
