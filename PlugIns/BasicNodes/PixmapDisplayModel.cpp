#include "PixmapDisplayModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "PixmapData.hpp"

PixmapDisplayModel::
PixmapDisplayModel()
    : PBNodeDataModel( _model_name ),
      mpQLabelImageDisplay(new QLabel("Image will appear here"))
{
    mpQLabelImageDisplay->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    QFont f = mpQLabelImageDisplay->font();
    f.setBold(true);
    f.setItalic(true);

    mpQLabelImageDisplay->setFont(f);

    mpQLabelImageDisplay->setFixedSize(200, 200);

    mpQLabelImageDisplay->installEventFilter(this);
    miImageDisplayWidth = mpQLabelImageDisplay->width();
    miImageDisplayHeight = mpQLabelImageDisplay->height();
}

unsigned int
PixmapDisplayModel::
nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType)
    {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;
        break;

    default:
        break;
    }

    return result;
}

bool
PixmapDisplayModel::
eventFilter(QObject *object, QEvent *event)
{
    if (object == mpQLabelImageDisplay)
    {
        if (event->type() == QEvent::Resize)
        {
            miImageDisplayWidth = mpQLabelImageDisplay->width();
            miImageDisplayHeight = mpQLabelImageDisplay->height();
            auto d = std::dynamic_pointer_cast<PixmapData>(mpNodeData);
            if (d)
                mpQLabelImageDisplay->setPixmap(d->pixmap().scaled(miImageDisplayWidth, miImageDisplayHeight, Qt::KeepAspectRatio));
        }
    }

    return false;
}

NodeDataType
PixmapDisplayModel::
dataType(PortType, PortIndex) const
{
    return PixmapData().type();
}

void
PixmapDisplayModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    mpNodeData = nodeData;

    if (mpNodeData)
    {
        auto d = std::dynamic_pointer_cast<PixmapData>(mpNodeData);
        mpQLabelImageDisplay->setPixmap(d->pixmap().scaled(miImageDisplayWidth, miImageDisplayHeight, Qt::KeepAspectRatio));
    }
    else
    {
        mpQLabelImageDisplay->setPixmap( QPixmap() );
    }

    Q_EMIT dataUpdated(0);
}

const QString PixmapDisplayModel::_category = QString( "Display" );

const QString PixmapDisplayModel::_model_name = QString( "Pixmap Display" );
