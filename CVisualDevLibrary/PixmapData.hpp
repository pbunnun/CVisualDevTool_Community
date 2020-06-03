#pragma once

#include <QtGui/QPixmap>

#include <nodes/NodeDataModel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class PixmapData : public NodeData
{
public:

    PixmapData()
        : mQPixmap()
    {}

    PixmapData(QPixmap const &pixmap)
        : mQPixmap(pixmap)
    {}

    NodeDataType
    type() const override
    {
        //       id      name
        return {"pixmap", "QPx"};
    }

    QPixmap
    pixmap() const { return mQPixmap; }

private:

    QPixmap mQPixmap;
};
