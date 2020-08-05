QT += widgets opengl

TEMPLATE = lib

CONFIG += c++14
INCLUDEPATH += $$PWD/../NodeEditor/include $$PWD/../NodeEditor/include/nodes $$PWD/../QtPropertyBrowserLibrary $$PWD/../QtPropertyBrowserLibrary/qtpropertybrowser

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS NODE_EDITOR_SHARED CVISUALDEV_LIBRARY

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PBFlowScene.cpp \
    PBFlowView.cpp \
    PBImageDisplayWidget.cpp \
    PBNodeDataModel.cpp \
    PBTreeWidget.cpp \
    PluginInterface.cpp \
    MainWindow.cpp

HEADERS += \
    ArrayData.hpp \
    CVImageData.hpp \
    CVisualDevLibrary.hpp \
    CVPointData.hpp \
    CVRectData.hpp \
    CVSizeData.hpp \
    DoubleData.hpp \
    FloatData.hpp \
    InformationData.hpp \
    IntegerData.hpp \
    MainWindow.hpp \
    PBFlowScene.hpp \
    PBFlowView.hpp \
    PBImageDisplayWidget.hpp \
    PBNodeDataModel.hpp \
    PBTreeWidget.hpp \
    PluginInterface.hpp \
    Property.hpp \
    StdStringData.hpp \
    StdVectorData.hpp \
    SyncData.hpp

FORMS += \
    MainWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../NodeEditor/release/ -lNodeEditor \
    -L$$OUT_PWD/../QtPropertyBrowserLibrary/release/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430 -LC:\opencv\build\x64\vc15\bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../NodeEditor/debug/ -lNodeEditor \
    -L$$OUT_PWD/../QtPropertyBrowserLibrary/debug/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430d -LC:\opencv\build\x64\vc15\bin
else:unix:!macx {
        LIBS += -L$$OUT_PWD/../NodeEditor/ -lNodeEditor -L$$OUT_PWD/../QtPropertyBrowserLibrary -lQtPropertyBrowserLibrary -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }
else:macx: {
        LIBS += -L../CVisualDev/CVisualDev.app/Contents/MacOS/ -lNodeEditor -lQtPropertyBrowserLibrary -L/usr/local/lib -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }

win32: INCLUDEPATH += C:\opencv\build\include
else:macx: INCLUDEPATH += /usr/local/Cellar/opencv/4.3.0/include/opencv4

macx: DESTDIR = ../CVisualDev/CVisualDev.app/Contents/MacOS

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
