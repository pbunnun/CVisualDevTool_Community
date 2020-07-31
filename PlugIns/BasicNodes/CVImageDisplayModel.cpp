#include "CVImageDisplayModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>
#include <opencv2/imgproc.hpp>

#include "CVImageData.hpp"

CVImageDisplayModel::
CVImageDisplayModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new PBImageDisplayWidget )
{
    mpEmbeddedWidget->installEventFilter( this );
    mpSyncData = std::make_shared<SyncData>();
    mpEmbeddedWidget->setMouseTracking(true);
}

unsigned int
CVImageDisplayModel::
nPorts(PortType portType) const
{
    if( portType == PortType::In )
        return 1;
    else if( portType == PortType::Out)
        return 1;
    else
        return 0;
}

bool
CVImageDisplayModel::
eventFilter(QObject *object, QEvent *event)
{
    if( object == mpEmbeddedWidget )
    {
        if( event->type() == QEvent::Resize )
            display_image();
    }
    return false;
}


NodeDataType
CVImageDisplayModel::
dataType( PortType portType, PortIndex ) const
{
    if(portType == PortType::In)
        return CVImageData().type();
    else
        return SyncData().type();
}

std::shared_ptr<NodeData>
CVImageDisplayModel::
outData(PortIndex)
{
    return mpSyncData;
}

void
CVImageDisplayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    if (nodeData)
    {
        mpSyncData->state() = false;
        Q_EMIT dataUpdated(0);
        mpNodeData = nodeData;
        display_image();
        mpSyncData->state() = true;
        Q_EMIT dataUpdated(0);
    }
}

void
CVImageDisplayModel::
display_image()
{
    auto d = std::dynamic_pointer_cast< CVImageData >( mpNodeData );
    if ( d )
        mpEmbeddedWidget->Display( d->image() );
}


const QString CVImageDisplayModel::_category = QString( "Display" );

const QString CVImageDisplayModel::_model_name = QString( "CV Image Display" );
