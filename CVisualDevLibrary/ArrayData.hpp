#ifndef ARRAYDATA_HPP
#define ARRAYDATA_HPP

#include <array>
#include <vector>

#include <nodes/NodeDataModel>
#include "InformationData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;

template<typename T, unsigned int x>
class ArrayData : public InformationData
{
public:

    ArrayData()
        : mapData(nullptr),
          miSize(0)
    {}

    ArrayData( const T (&arr)[x] )
        : mapData( new T[x] ),
          miSize(x)
    {
        for(unsigned int i=0; i<miSize; i++)
        {
            mapData[i] = arr[i];
        }
    }

    ArrayData( const std::array<T,x>& stdArr )
        : mapData( new T[x] ),
          miSize(x)
    {
        for(unsigned int i=0; i<miSize; i++)
        {
            mapData[i] = stdArr[i];
        }
    }

    ArrayData( const std::vector<T>& vec )
    {
        for(unsigned int i=0; i<miSize; i++)
        {
            mapData[i] = vec[i];
        }
    }

    virtual ~ArrayData()
    {
        if(mapData)
            delete[] mapData;
    }

    NodeDataType
    type() const override
    {
        return { "information", "Arr" };
    }

    T* const
    array()
    {
        return mapData;
    }

    T elementInstance() const
    {
        return T();
    }

    unsigned int
    size() const
    {
        return miSize;
    }

    void set_information() override
    {
        mQSData = QString("Will be updated soon -_-");
    }

private:
    T* const mapData {nullptr};
    const unsigned int miSize;

};

#endif // ARRAYDATA_HPP
