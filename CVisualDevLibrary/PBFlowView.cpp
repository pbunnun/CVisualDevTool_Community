#include "PBFlowView.hpp"
#include <nodes/FlowScene>
#include <nodes/Node>
#include <QMimeData>
#include <QDebug>
#include <QMenu>

PBFlowView::PBFlowView(QWidget *parent) : QtNodes::FlowView(parent)
{
    setAcceptDrops(true);
}

void
PBFlowView::
dragMoveEvent(QDragMoveEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    event->accept();
}


void
PBFlowView::
dropEvent(QDropEvent *event)
{
    auto type = scene()->registry().create(event->mimeData()->text());
    if(type)
    {
        auto& node = scene()->createNode(std::move(type));

        node.nodeDataModel()->late_constructor();

        QPoint pos = event->pos();

        QPointF posView = this->mapToScene(pos) - QPointF(node.nodeGeometry().width()/2, 0);

        node.nodeGraphicsObject().setPos(posView);

        scene()->nodePlaced(node);
    }
    event->accept();
}


void
PBFlowView::
contextMenuEvent(QContextMenuEvent *event)
{
    if(!itemAt(event->pos()) || scene()->selectedItems().isEmpty())
    {
        QtNodes::FlowView::contextMenuEvent(event);
        return;
    }

    QMenu modelMenu;

    auto * deleteAction = new QAction(tr("Delete"), this);
    connect(deleteAction, &QAction::triggered, this, &QtNodes::FlowView::deleteSelectedNodes);

    modelMenu.addAction(deleteAction);

    modelMenu.exec(event->globalPos());
}

void
PBFlowView::
center_on( const Node * pNode )
{
    QPointF difference(10, 10);
    int max_loop = 5;
    while( difference.manhattanLength() > 2 && max_loop-- > 0 )
    {
        auto nodeRect = pNode->nodeGraphicsObject().sceneBoundingRect();
        auto pos = QPointF( nodeRect.x() + nodeRect.width()/2., nodeRect.y() + nodeRect.height()/2. );
        /*
         * mapToScene accepts only an integer value caused precision error when the scene is in zoom-in.
         * To reduce the error, this loop is needed to call many times so that the error will convert to zero.
         */
        auto target = mapToScene( width()/2, height()/2 );
        difference = pos - target;
        setSceneRect( sceneRect().translated( difference.x(), difference.y() ) );
    }

}
