TEMPLATE	 = lib
DEFINES		+= QT_DEPRECATED_WARNINGS NODE_EDITOR_SHARED QT_NO_KEYWORDS
CONFIG		+= c++14 plugin
QT		+= widgets
INCLUDEPATH += ../../CVisualDev ../../NodeEditor/include ../../NodeEditor/include/nodes ../../CVisualDevLibrary ../../QtPropertyBrowserLibrary ../../QtPropertyBrowserLibrary/qtpropertybrowser

HEADERS		+= \
            BasicNodePlugin.hpp \
            CVCameraEmbeddedWidget.hpp \
            CVCameraModel.hpp \
            CVImageDisplayModel.hpp \
            CVImageLoaderModel.hpp \
            CVVDOLoaderModel.hpp \
            CannyEdgeModel.hpp \
            ColorSpaceModel.hpp \
            CreateHistogramModel.hpp \
            DrawContourModel.hpp \
            ErodeAndDilateEmbeddedWidget.hpp \
            ErodeAndDilateModel.hpp \
            Filter2DModel.hpp \
            GaussianBlurModel.hpp \
            InvertGrayModel.hpp \
            PixmapDisplayModel.hpp \
            RGBsetValueEmbeddedWidget.hpp \
            RGBsetValueModel.hpp \
            RGBtoGrayModel.hpp \
            SobelAndScharrEmbeddedWidget.hpp \
            SobelAndScharrModel.hpp \
            TemplateModel.hpp \
            TemplateEmbeddedWidget.hpp \
            Test_SharpenModel.hpp \
            ThresholdingModel.hpp
SOURCES		+= \
            BasicNodePlugin.cpp \
            CVCameraEmbeddedWidget.cpp \
            CVCameraModel.cpp \
            CVImageDisplayModel.cpp \
            CVImageLoaderModel.cpp \
            CVVDOLoaderModel.cpp \
            CannyEdgeModel.cpp \
            ColorSpaceModel.cpp \
            CreateHistogramModel.cpp \
            DrawContourModel.cpp \
            ErodeAndDilateEmbeddedWidget.cpp \
            ErodeAndDilateModel.cpp \
            Filter2DModel.cpp \
            GaussianBlurModel.cpp \
            InvertGrayModel.cpp \
            PixmapDisplayModel.cpp \
            RGBsetValueEmbeddedWidget.cpp \
            RGBsetValueModel.cpp \
            RGBtoGrayModel.cpp \
            SobelAndScharrEmbeddedWidget.cpp \
            SobelAndScharrModel.cpp \
            TemplateModel.cpp \
            TemplateEmbeddedWidget.cpp \
            Test_SharpenModel.cpp \
            ThresholdingModel.cpp
TARGET		= $$qtLibraryTarget(plugin_Basics)

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../NodeEditor/release/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/release/ -lCVisualDevLibrary \
    -L$$PWD/../../QtPropertyBrowserLibrary/release/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430 -LC:\opencv\build\x64\vc15\bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../NodeEditor/debug/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/debug/ -lCVisualDevLibrary \
    -L$$PWD/../../QtPropertyBrowserLibrary/debug/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430d -LC:\opencv\build\x64\vc15\bin
else:unix:!macx {
        LIBS += -L$$PWD/../../NodeEditor/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary -lCVisualDevLibrary -L$$PWD/../../QtPropertyBrowserLibrary -lQtPropertyBrowserLibrary -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }
else:macx: {
        LIBS += -L../../CVisualDev/CVisualDev.app/Contents/MacOS/ -lNodeEditor -lCVisualDevLibrary -lQtPropertyBrowserLibrary -L/usr/local/lib -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }

win32:CONFIG(release, debug|release): DESTDIR	= ../../CVisualDev/release/plugins
else:win32:CONFIG(debug, debug|release): DESTDIR	= ../../CVisualDev/debug/plugins
else:unix:!macx:CONFIG(release, debug|release): DESTDIR	= ../../CVisualDev/plugins
else:unix:!macx:CONFIG(debug, debug|release): DESTDIR	= ../../CVisualDev/plugins
else:macx: DESTDIR = ../../CVisualDev/CVisualDev.app/Contents/MacOS/plugins

win32: INCLUDEPATH += C:\opencv\build\include
else:macx: INCLUDEPATH += /usr/local/Cellar/opencv/4.3.0/include/opencv4

RESOURCES += \
    resources/basic_resources.qrc

FORMS += \
    CVCameraEmbeddedWidget.ui \
    ErodeAndDilateEmbeddedWidget.ui \
    RGBsetValueEmbeddedWidget.ui \
    SobelAndScharrEmbeddedWidget.ui \
    TemplateEmbeddedWidget.ui

