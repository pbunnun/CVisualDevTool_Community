TEMPLATE = subdirs

SUBDIRS += \
    QtPropertyBrowserLibrary \
    NodeEditor \
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
    ./CVisualDevLibrary/MainWindow.hpp

CVisualDev.depends = QtPropertyBrowserLibrary NodeEditor
PlugIns.depends = QtPropertyBrowserLibrary NodeEditor
