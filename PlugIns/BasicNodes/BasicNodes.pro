TEMPLATE	 = lib
DEFINES		+= QT_DEPRECATED_WARNINGS NODE_EDITOR_SHARED QT_NO_KEYWORDS
CONFIG		+= c++14 plugin
QT		+= widgets
INCLUDEPATH += ../../CVisualDev ../../NodeEditor/include ../../NodeEditor/include/nodes ../../CVisualDevLibrary ../../QtPropertyBrowserLibrary ../../QtPropertyBrowserLibrary/qtpropertybrowser

HEADERS		+= \
            BasicNodePlugin.hpp \
            BitwiseOperationEmbeddedWidget.hpp \
            BitwiseOperationModel.hpp \
            BlendImagesEmbeddedWidget.hpp \
            BlendImagesModel.hpp \
            FaceDetectionEmbeddedWidget.hpp \
            CVCameraEmbeddedWidget.hpp \
            CVCameraModel.hpp \
            CVImageDisplayModel.hpp \
            CVImageLoaderModel.hpp \
            CVImagePropertiesModel.hpp \
            CVVDOLoaderModel.hpp \
            CannyEdgeModel.hpp \
            ColorSpaceModel.hpp \
            CreateHistogramModel.hpp \
            DistanceTransformModel.hpp \
            DrawContourModel.hpp \
            ErodeAndDilateEmbeddedWidget.hpp \
            ErodeAndDilateModel.hpp \
            FaceDetectionModel.hpp \
            Filter2DModel.hpp \
            FloodFillEmbeddedWidget.hpp \
            FloodFillModel.hpp \
            GaussianBlurModel.hpp \
            HoughCircleTransfromModel.hpp \
            ImageROIModel.hpp \
            InformationDisplayModel.hpp \
            InvertGrayModel.hpp \
            MakeBorderModel.hpp \
            MorphologicalTransformationModel.hpp \
            RGBsetValueEmbeddedWidget.hpp \
            RGBsetValueModel.hpp \
            RGBtoGrayModel.hpp \
            SobelAndScharrEmbeddedWidget.hpp \
            SobelAndScharrModel.hpp \
            SplitImageModel.hpp \
            TemplateMatchingModel.hpp \
            TemplateModel.hpp \
            TemplateEmbeddedWidget.hpp \
            Test_SharpenModel.hpp \
            ThresholdingModel.hpp
SOURCES		+= \
            BasicNodePlugin.cpp \
            BitwiseOperationEmbeddedWidget.cpp \
            BitwiseOperationModel.cpp \
            BlendImagesEmbeddedWidget.cpp \
            BlendImagesModel.cpp \
            FaceDetectionEmbeddedWidget.cpp \
            CVCameraEmbeddedWidget.cpp \
            CVCameraModel.cpp \
            CVImageDisplayModel.cpp \
            CVImageLoaderModel.cpp \
            CVImagePropertiesModel.cpp \
            CVVDOLoaderModel.cpp \
            CannyEdgeModel.cpp \
            ColorSpaceModel.cpp \
            CreateHistogramModel.cpp \
            DistanceTransformModel.cpp \
            DrawContourModel.cpp \
            ErodeAndDilateEmbeddedWidget.cpp \
            ErodeAndDilateModel.cpp \
            FaceDetectionModel.cpp \
            Filter2DModel.cpp \
            FloodFillEmbeddedWidget.cpp \
            FloodFillModel.cpp \
            GaussianBlurModel.cpp \
            HoughCircleTransfromModel.cpp \
            ImageROIModel.cpp \
            InformationDisplayModel.cpp \
            InvertGrayModel.cpp \
            MakeBorderModel.cpp \
            MorphologicalTransformationModel.cpp \
            RGBsetValueEmbeddedWidget.cpp \
            RGBsetValueModel.cpp \
            RGBtoGrayModel.cpp \
            SobelAndScharrEmbeddedWidget.cpp \
            SobelAndScharrModel.cpp \
            SplitImageModel.cpp \
            TemplateMatchingModel.cpp \
            TemplateModel.cpp \
            TemplateEmbeddedWidget.cpp \
            Test_SharpenModel.cpp \
            ThresholdingModel.cpp
TARGET		= $$qtLibraryTarget(plugin_Basics)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../NodeEditor/release/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/release/ -lCVisualDevLibrary \
    -L$$OUT_PWD/../../QtPropertyBrowserLibrary/release/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430 -LC:\opencv\build\x64\vc15\bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../NodeEditor/debug/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/debug/ -lCVisualDevLibrary \
    -L$$OUT_PWD/../../QtPropertyBrowserLibrary/debug/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430d -LC:\opencv\build\x64\vc15\bin
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
    BitwiseOperationEmbeddedWidget.ui \
    BlendImagesEmbeddedWidget.ui \
    CVCameraEmbeddedWidget.ui \
    ErodeAndDilateEmbeddedWidget.ui \
    FaceDetectionEmbeddedWidget.ui \
    FloodFillEmbeddedWidget.ui \
    RGBsetValueEmbeddedWidget.ui \
    SobelAndScharrEmbeddedWidget.ui \
    TemplateEmbeddedWidget.ui

