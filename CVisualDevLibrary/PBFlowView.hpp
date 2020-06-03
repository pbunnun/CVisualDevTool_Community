#ifndef PBFLOWVIEW_H
#define PBFLOWVIEW_H

#pragma once

#include "CVisualDevLibrary.hpp"

#include "nodes/internal/Export.hpp"
#include <nodes/FlowView>
#include <nodes/Node>
#include <QDragMoveEvent>

using QtNodes::Node;

class CVISUALDEVSHAREDLIB_EXPORT PBFlowView : public QtNodes::FlowView
{
    Q_OBJECT
public:
    explicit PBFlowView( QWidget *parent = nullptr );
    void center_on( const Node * pNode );

protected Q_SLOTS:
    void contextMenuEvent( QContextMenuEvent *event ) override;

    void dragMoveEvent( QDragMoveEvent *event ) override;

    void dropEvent( QDropEvent *event ) override;
};

#endif // PBFLOWVIEW_H
