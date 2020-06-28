QT += widgets core gui opengl

TEMPLATE = lib
DEFINES += NODE_EDITOR_EXPORTS NODE_EDITOR_SHARED QT_NO_KEYWORDS nodes_EXPORTS

CONFIG += c++14
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
# DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Connection.cpp \
    src/ConnectionBlurEffect.cpp \
    src/ConnectionGeometry.cpp \
    src/ConnectionGraphicsObject.cpp \
    src/ConnectionPainter.cpp \
    src/ConnectionState.cpp \
    src/ConnectionStyle.cpp \
    src/DataModelRegistry.cpp \
    src/FlowScene.cpp \
    src/FlowView.cpp \
    src/FlowViewStyle.cpp \
    src/Node.cpp \
    src/NodeConnectionInteraction.cpp \
    src/NodeDataModel.cpp \
    src/NodeGeometry.cpp \
    src/NodeGraphicsObject.cpp \
    src/NodePainter.cpp \
    src/NodeState.cpp \
    src/NodeStyle.cpp \
    src/Properties.cpp \
    src/StyleCollection.cpp

HEADERS += \
    include/nodes/Connection \
    include/nodes/ConnectionStyle \
    include/nodes/DataModelRegistry \
    include/nodes/FlowScene \
    include/nodes/FlowView \
    include/nodes/FlowViewStyle \
    include/nodes/Node \
    include/nodes/NodeData \
    include/nodes/NodeDataModel \
    include/nodes/NodeGeometry \
    include/nodes/NodePainterDelegate \
    include/nodes/NodeState \
    include/nodes/NodeStyle \
    include/nodes/TypeConverter \
    include/nodes/internal/Compiler.hpp \
    include/nodes/internal/Connection.hpp \
    include/nodes/internal/ConnectionGeometry.hpp \
    include/nodes/internal/ConnectionGraphicsObject.hpp \
    include/nodes/internal/ConnectionState.hpp \
    include/nodes/internal/ConnectionStyle.hpp \
    include/nodes/internal/DataModelRegistry.hpp \
    include/nodes/internal/Export.hpp \
    include/nodes/internal/FlowScene.hpp \
    include/nodes/internal/FlowView.hpp \
    include/nodes/internal/FlowViewStyle.hpp \
    include/nodes/internal/Node.hpp \
    include/nodes/internal/NodeData.hpp \
    include/nodes/internal/NodeDataModel.hpp \
    include/nodes/internal/NodeGeometry.hpp \
    include/nodes/internal/NodeGraphicsObject.hpp \
    include/nodes/internal/NodePainterDelegate.hpp \
    include/nodes/internal/NodeState.hpp \
    include/nodes/internal/NodeStyle.hpp \
    include/nodes/internal/OperatingSystem.hpp \
    include/nodes/internal/PortType.hpp \
    include/nodes/internal/QStringStdHash.hpp \
    include/nodes/internal/QUuidStdHash.hpp \
    include/nodes/internal/Serializable.hpp \
    include/nodes/internal/Style.hpp \
    include/nodes/internal/TypeConverter.hpp \
    include/nodes/internal/memory.hpp \
    src/ConnectionBlurEffect.hpp \
    src/ConnectionPainter.hpp \
    src/NodeConnectionInteraction.hpp \
    src/NodePainter.hpp \
    src/Properties.hpp \
    src/StyleCollection.hpp

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

macx: DESTDIR = ../CVisualDev/CVisualDev.app/Contents/MacOS/

DISTFILES += \
    resources/DefaultStyle.json \
    resources/convert.png

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += include/nodes/internal
