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
    ./CVisualDevLibrary/PluginInterface.hpp \
    ./CVisualDevLibrary/PBFlowView.hpp \
    ./CVisualDevLibrary/PBTreeWidget.hpp \
    ./CVisualDevLibrary/Property.hpp \
    ./CVisualDevLibrary/MainWindow.hpp \
    ./CVisualDevLibrary/CVPointData.hpp \
    ./CVisualDevLibrary/CVRectData.hpp \
    ./CVisualDevLibrary/CVSizeData.hpp \
    ./CVisualDevLibrary/InformationData.hpp \
    ./CVisualDevLibrary/IntegerData.hpp \
    ./CVisualDevLibrary/StdStringData.hpp \
    ./CVisualDevLibrary/SyncData.hpp \
    ./CVisualDevLibrary/DoubleData.hpp \
    ./CVisualDevLibrary/FloatData.hpp

CVisualDev.depends = QtPropertyBrowserLibrary NodeEditor
PlugIns.depends = QtPropertyBrowserLibrary NodeEditor
