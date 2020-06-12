TEMPLATE = subdirs

SUBDIRS += \
    Plugins \
    CVisualDev

HEADERS += \
    ./CVisualDevLibrary/CVImageData.hpp \
    ./CVisualDevLibrary/CVisualDevLibrary.hpp \
    ./CVisualDevLibrary/PBFlowScene.hpp \
    ./CVisualDevLibrary/PBImageDisplayWidget.hpp \
    ./CVisualDevLibrary/PBNodeDataModel.hpp \
    ./CVisualDevLibrary/PixmapData.hpp \
    ./CVisualDevLibrary/PluginInterface.hpp \
    ./CVisualDevLibrary/PBFlowView.hpp \
    ./CVisualDevLibrary/PBTreeWidget.hpp \
    ./CVisualDevLibrary/Property.hpp \
    ./NodeEditor/include/nodes/Connection \
    ./NodeEditor/include/nodes/ConnectionStyle \
    ./NodeEditor/include/nodes/DataModelRegistry \
    ./NodeEditor/include/nodes/FlowScene \
    ./NodeEditor/include/nodes/FlowView \
    ./NodeEditor/include/nodes/FlowViewStyle \
    ./NodeEditor/include/nodes/Node \
    ./NodeEditor/include/nodes/NodeData \
    ./NodeEditor/include/nodes/NodeDataModel \
    ./NodeEditor/include/nodes/NodeGeometry \
    ./NodeEditor/include/nodes/NodePainterDelegate \
    ./NodeEditor/include/nodes/NodeState \
    ./NodeEditor/include/nodes/NodeStyle \
    ./NodeEditor/include/nodes/TypeConverter \
    ./NodeEditor/include/nodes/internal/Compiler.hpp \
    ./NodeEditor/include/nodes/internal/Connection.hpp \
    ./NodeEditor/include/nodes/internal/ConnectionGeometry.hpp \
    ./NodeEditor/include/nodes/internal/ConnectionGraphicsObject.hpp \
    ./NodeEditor/include/nodes/internal/ConnectionState.hpp \
    ./NodeEditor/include/nodes/internal/ConnectionStyle.hpp \
    ./NodeEditor/include/nodes/internal/DataModelRegistry.hpp \
    ./NodeEditor/include/nodes/internal/Export.hpp \
    ./NodeEditor/include/nodes/internal/FlowScene.hpp \
    ./NodeEditor/include/nodes/internal/FlowView.hpp \
    ./NodeEditor/include/nodes/internal/FlowViewStyle.hpp \
    ./NodeEditor/include/nodes/internal/Node.hpp \
    ./NodeEditor/include/nodes/internal/NodeData.hpp \
    ./NodeEditor/include/nodes/internal/NodeDataModel.hpp \
    ./NodeEditor/include/nodes/internal/NodeGeometry.hpp \
    ./NodeEditor/include/nodes/internal/NodeGraphicsObject.hpp \
    ./NodeEditor/include/nodes/internal/NodePainterDelegate.hpp \
    ./NodeEditor/include/nodes/internal/NodeState.hpp \
    ./NodeEditor/include/nodes/internal/NodeStyle.hpp \
    ./NodeEditor/include/nodes/internal/OperatingSystem.hpp \
    ./NodeEditor/include/nodes/internal/PortType.hpp \
    ./NodeEditor/include/nodes/internal/QStringStdHash.hpp \
    ./NodeEditor/include/nodes/internal/QUuidStdHash.hpp \
    ./NodeEditor/include/nodes/internal/Serializable.hpp \
    ./NodeEditor/include/nodes/internal/Style.hpp \
    ./NodeEditor/include/nodes/internal/TypeConverter.hpp \
    ./NodeEditor/include/nodes/internal/memory.hpp \
    ./NodeEditor/src/ConnectionBlurEffect.hpp \
    ./NodeEditor/src/ConnectionPainter.hpp \
    ./NodeEditor/src/NodeConnectionInteraction.hpp \
    ./NodeEditor/src/NodePainter.hpp \
    ./NodeEditor/src/Properties.hpp \
    ./NodeEditor/src/StyleCollection.hpp
