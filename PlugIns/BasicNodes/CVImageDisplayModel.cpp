#include "CVImageDisplayModel.hpp"

#include <QtCore/QEvent>
#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "CVImageData.hpp"

CVImageDisplayModel::
CVImageDisplayModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new PBImageDisplayWidget() )
{
    mpEmbeddedWidget->installEventFilter( this );
}

unsigned int
CVImageDisplayModel::
nPorts(PortType portType) const
{
    if( portType == PortType::In )
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
dataType( PortType, PortIndex ) const
{
    return CVImageData().type();
}

void
CVImageDisplayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    mpNodeData = nodeData;
    display_image();
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
