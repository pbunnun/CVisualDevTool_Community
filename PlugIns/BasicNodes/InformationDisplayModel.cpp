#include "InformationDisplayModel.hpp"

#include "nodes/DataModelRegistry"
#include "InformationData.hpp"

InformationDisplayModel::
InformationDisplayModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new QPlainTextEdit() )
{
    mpEmbeddedWidget->setMaximumBlockCount( 100 );
    mpEmbeddedWidget->setReadOnly( true );
}

unsigned int
InformationDisplayModel::
nPorts(PortType portType) const
{
    if( portType == PortType::In )
        return 1;
    else
        return 0;
}

NodeDataType
InformationDisplayModel::
dataType( PortType, PortIndex ) const
{
    return InformationData().type();
}

void
InformationDisplayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
{
    if( !isEnable() )
        return;

    mpNodeData = nodeData;
    auto d = std::dynamic_pointer_cast< InformationData >( mpNodeData );
    if( d )
    {
        mpEmbeddedWidget->appendPlainText( d->information() );
    }
}

const QString InformationDisplayModel::_category = QString( "Display" );

const QString InformationDisplayModel::_model_name = QString( "Info Display" );
