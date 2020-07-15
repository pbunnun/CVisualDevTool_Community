//#include "CVImageInteractiveDisplayModel.hpp"

//#include <QtCore/QEvent>
//#include <QtCore/QDir>

//#include <QtWidgets/QFileDialog>

//#include <nodes/DataModelRegistry>

//#include "CVImageData.hpp"

//CVImageInteractiveDisplayModel::
//CVImageInteractiveDisplayModel()
//    : PBNodeDataModel( _model_name, true ),
//      mpEmbeddedWidget( new PBImageInteractiveDisplayWidget() )
//{
//    mpEmbeddedWidget->installEventFilter( this );
//    mpNodeData = std::make_shared<NodeData>();
//    mpSyncData = std::make_shared<SyncData>();
//}

//unsigned int
//CVImageInteractiveDisplayModel::
//nPorts(PortType portType) const
//{
//    if( portType == PortType::In )
//        return 1;
//    else if( portType == PortType::Out)
//        return 2;
//    else
//        return 0;
//}

//bool
//CVImageInteractiveDisplayModel::
//eventFilter(QObject *object, QEvent *event)
//{
//    if( object == mpEmbeddedWidget )
//    {
//        if( event->type() == QEvent::Resize )
//            pinpoint_image();
//        else if(event->type() == QEvent::MouseButtonPress)
//            pinpoint_image();
//    }
//    return false;
//}


//NodeDataType
//CVImageInteractiveDisplayModel::
//dataType( PortType, PortIndex portIndex) const
//{
//    if(portIndex == 0)
//        return CVImageData().type();
//    else
//        return SyncData().type();
//}

//std::shared_ptr<NodeData>
//CVImageInteractiveDisplayModel::
//outData(PortIndex I)
//{
//    if(I == 0)
//        return mpNodeData;
//    else if(I == 1)
//        return mpSyncData;
//    return nullptr;
//}

//void
//CVImageInteractiveDisplayModel::
//setInData( std::shared_ptr< NodeData > nodeData, PortIndex )
//{
//    if( !isEnable() )
//        return;

//    if (nodeData)
//    {
//        mpSyncData->emit();
//        Q_EMIT dataUpdated(0);
//        mpNodeData = nodeData;
//        pinpoint_image();
//        mpSyncData->emit();
//        Q_EMIT dataUpdated(0);
//    }
//}

//void
//CVImageInteractiveDisplayModel::
//pinpoint_image()
//{
//    auto d = std::dynamic_pointer_cast< CVImageData >( mpNodeData );
//    if ( d )
//    {
//        const auto point = mpEmbeddedWidget->Coordinate();
//        cv::Scalar color(255,255,0);
//        cv::line(d->image(),
//                 cv::Point(0,point.y),
//                 cv::Point(d->image().cols,point.y),
//                 color,
//                 5,
//                 cv::LINE_AA);
//        cv::line(d->image(),
//                 cv::Point(point.x,0),
//                 cv::Point(point.x,d->image().rows),
//                 color,
//                 5,
//                 cv::LINE_AA);
//        mpEmbeddedWidget->Display( d->image() );
//    }
//}

//const QString CVImageInteractiveDisplayModel::_category = QString( "Display" );

//const QString CVImageInteractiveDisplayModel::_model_name = QString( "CV Image Interactive Display" );
