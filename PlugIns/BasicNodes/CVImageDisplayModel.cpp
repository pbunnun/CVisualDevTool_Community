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
        {
            auto d = std::dynamic_pointer_cast< CVImageData >( mpNodeData );
            if( d )
            {
                if( d->image().channels() == 1 )
                    mpEmbeddedWidget->DisplayGray( d->image() );
                else
                    mpEmbeddedWidget->DisplayRGB( d->image() );
            }
        }
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
    if( !mbEnable )
        return;

    mpNodeData = nodeData;

    if ( mpNodeData )
    {
        auto d = std::dynamic_pointer_cast< CVImageData >( mpNodeData );

        if (d)
        {
            if( d->image().channels() == 1 )
                mpEmbeddedWidget->DisplayGray( d->image() );
            else
                mpEmbeddedWidget->DisplayRGB( d->image() );
        }
    }
}

const QString CVImageDisplayModel::_category = QString( "Display" );

const QString CVImageDisplayModel::_model_name = QString( "CV Image Display" );
