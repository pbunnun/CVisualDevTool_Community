#include "InformationDisplayModel.hpp"

#include "nodes/DataModelRegistry"
#include "InformationData.hpp"
#include "SyncData.hpp"

InformationDisplayModel::
InformationDisplayModel()
    : PBNodeDataModel( _model_name, true ),
      mpEmbeddedWidget( new QPlainTextEdit() )
{
    mpEmbeddedWidget->setMaximumBlockCount( 100 ); //May be modified to be customized by the user
    mpEmbeddedWidget->setReadOnly( true );
}

unsigned int
InformationDisplayModel::
nPorts(PortType portType) const
{
    if( portType == PortType::In )
        return 2;
    else
        return 0;
}

NodeDataType
InformationDisplayModel::
dataType( PortType, PortIndex portIndex) const
{
    if(portIndex == 0)
    {
        return InformationData().type();
    }
    else if(portIndex == 1)
    {
        return SyncData().type();
    }
    return NodeDataType();
}

void
InformationDisplayModel::
setInData( std::shared_ptr< NodeData > nodeData, PortIndex portIndex)
{
    if( !isEnable() )
        return;
    if(portIndex == 0)
    {
        mpNodeData = nodeData;
        auto d = std::dynamic_pointer_cast< InformationData >( mpNodeData );
        if( d )
        {
            d->set_information();
            mpEmbeddedWidget->appendPlainText( d->info() );
        }
    }
    if(portIndex == 1)
    {
        auto d = std::dynamic_pointer_cast< SyncData >( nodeData );
        if( d )
        {
            mpEmbeddedWidget->appendPlainText(d->state_str());
        }
    }
}

const QString InformationDisplayModel::_category = QString( "Display" );

const QString InformationDisplayModel::_model_name = QString( "Info Display" );
