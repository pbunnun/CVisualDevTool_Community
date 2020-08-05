TEMPLATE = subdirs

SUBDIRS += \
    QtPropertyBrowserLibrary \
    NodeEditor \
    Plugins \
    CVisualDevLibrary \
    CVisualDev

CVisualDevLibrary.depends = NodeEditor
CVisualDev.depends = QtPropertyBrowserLibrary NodeEditor
PlugIns.depends = QtPropertyBrowserLibrary NodeEditor
