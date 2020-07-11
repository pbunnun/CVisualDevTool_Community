#ifndef STDVECTORDATA_HPP
#define STDVECTORDATA_HPP

#include <vector>

#include <nodes/NodeDataModel>
#include "InformationData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

template<typename T>
class StdVectorData : public InformationData
{
public:

    StdVectorData()
        : mvData()
    {}

    StdVectorData( const std::vector<T>& vector )
        : mvData( vector )
    {}

    NodeDataType
    type() const override
    {
        return { "information", "Vec" };
    }

    std::vector<T> &
    vector()
    {
        return mvData;
    }

    T elementInstance() const
    {
        return T();
    }

    void set_information() override
    {
        mQSData = QString("Will be updated soon -_-");
    }

private:
    std::vector<T> mvData;

};

#endif // STDVECTORDATA_HPP
