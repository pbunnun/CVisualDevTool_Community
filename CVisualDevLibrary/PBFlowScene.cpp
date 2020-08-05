#include "PBFlowScene.hpp"
#include "PBNodeDataModel.hpp"
#include <QFile>
#include <QFileInfo>
#include <nodes/Node>

using QtNodes::PortType;

PBFlowScene::
PBFlowScene(QWidget *parent)
    : QtNodes::FlowScene(parent)
{

}

bool
PBFlowScene::
save(QString & sFilename) const
{
    if( !sFilename.isEmpty() )
    {
        QFile file(sFilename);
        if( file.open(QIODevice::WriteOnly) )
        {
            file.write(saveToMemory());
            return true;
        }
        else
            return false;
    }
    return false;
}

bool
PBFlowScene::
load(QString & sFilename)
{
    if( !QFileInfo::exists(sFilename) )
        return false;

    clearScene();

    QFile file(sFilename);
    if( !file.open(QIODevice::ReadOnly) )
        return false;

    QByteArray wholeFile = file.readAll();

    loadFromMemory(wholeFile);

    auto nodes = allNodes();
    for( auto node : nodes )
    {
        auto dataModel = static_cast< PBNodeDataModel *>( node->nodeDataModel() );
        if( dataModel->nPorts( PortType::In ) == 0 )
            dataModel->updateAllOutputPorts();
    }

    return true;
}
