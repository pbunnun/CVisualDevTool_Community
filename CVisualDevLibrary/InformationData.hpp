#ifndef INFORMATIONDATA_HPP
#define INFORMATIONDATA_HPP

#pragma once

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

class InformationData : public NodeData
{
public:
    InformationData()
    {
        msData = "No Information!";
    }

    InformationData( QString & info )
        : msData( info )
    {}

    NodeDataType
    type() const override
    {
        return { "information", "Inf" };
    }

    void
    set_information( const QString & inf )
    {
        msData = inf;
    }

    QString
    information() const
    {
        return msData;
    }

private:
    QString msData;
};

#endif // INFORMATIONDATA_HPP
