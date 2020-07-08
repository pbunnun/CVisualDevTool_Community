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
            ColorMapModel.hpp \
            ConnectedComponentsModel.hpp \
            ConvertDepthModel.hpp \
            DataGeneratorEmbeddedWidget.hpp \
            DataGeneratorModel.hpp \
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
            ImageROIEmbeddedWidget.hpp \
            ImageROIModel.hpp \
            InformationDisplayModel.hpp \
            InvertGrayModel.hpp \
            MakeBorderModel.hpp \
            MatrixOperationModel.hpp \
            MinMaxLocationModel.hpp \
            MorphologicalTransformationModel.hpp \
            NormalizationModel.hpp \
            PixelIterationModel.hpp \
            RGBsetValueEmbeddedWidget.hpp \
            RGBsetValueModel.hpp \
            RGBtoGrayModel.hpp \
            ScalarOperationModel.hpp \
            SobelAndScharrEmbeddedWidget.hpp \
            SobelAndScharrModel.hpp \
            SplitImageModel.hpp \
            SyncGateEmbeddedWidget.hpp \
            SyncGateModel.hpp \
            TemplateMatchingModel.hpp \
            TemplateModel.hpp \
            TemplateEmbeddedWidget.hpp \
            Test_SharpenModel.hpp \
            ThresholdingModel.hpp \
            WatershedModel.hpp
SOURCES		+= \
            BasicNodePlugin.cpp \
            BitwiseOperationEmbeddedWidget.cpp \
            BitwiseOperationModel.cpp \
            BlendImagesEmbeddedWidget.cpp \
            BlendImagesModel.cpp \
            ColorMapModel.cpp \
            ConnectedComponentsModel.cpp \
            ConvertDepthModel.cpp \
            DataGeneratorEmbeddedWidget.cpp \
            DataGeneratorModel.cpp \
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
            ImageROIEmbeddedWidget.cpp \
            ImageROIModel.cpp \
            InformationDisplayModel.cpp \
            InvertGrayModel.cpp \
            MakeBorderModel.cpp \
            MatrixOperationModel.cpp \
            MinMaxLocationModel.cpp \
            MorphologicalTransformationModel.cpp \
            NormalizationModel.cpp \
            PixelIterationModel.cpp \
            RGBsetValueEmbeddedWidget.cpp \
            RGBsetValueModel.cpp \
            RGBtoGrayModel.cpp \
            ScalarOperationModel.cpp \
            SobelAndScharrEmbeddedWidget.cpp \
            SobelAndScharrModel.cpp \
            SplitImageModel.cpp \
            SyncGateEmbeddedWidget.cpp \
            SyncGateModel.cpp \
            TemplateMatchingModel.cpp \
            TemplateModel.cpp \
            TemplateEmbeddedWidget.cpp \
            Test_SharpenModel.cpp \
            ThresholdingModel.cpp \
            WatershedModel.cpp
TARGET		= $$qtLibraryTarget(plugin_Basics)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../NodeEditor/release/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/release/ -lCVisualDevLibrary \
    -L$$OUT_PWD/../../QtPropertyBrowserLibrary/release/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430 -LC:\opencv\build\x64\vc15\bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../NodeEditor/debug/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary/debug/ -lCVisualDevLibrary \
    -L$$OUT_PWD/../../QtPropertyBrowserLibrary/debug/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world430d -LC:\opencv\build\x64\vc15\bin
else:unix:!macx {
        LIBS += -L$$PWD/../../NodeEditor/ -lNodeEditor -L$$PWD/../../CVisualDevLibrary -lCVisualDevLibrary -L$$PWD/../../QtPropertyBrowserLibrary -lQtPropertyBrowserLibrary -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }
else:macx:CONFIG(release, debug|release): {
        LIBS += -L../../CVisualDev/CVisualDev.app/Contents/MacOS/ -lNodeEditor -lQtPropertyBrowserLibrary -L$$PWD/../../CVisualDevLibrary/release/ -lCVisualDevLibrary -L/usr/local/lib -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio -lopencv_face -lopencv_objdetect
    }
else:macx:CONFIG(debug, debug|release): {
        LIBS += -L../../CVisualDev/CVisualDev.app/Contents/MacOS/ -lNodeEditor -lQtPropertyBrowserLibrary -L$$PWD/../../CVisualDevLibrary/debug/ -lCVisualDevLibrary -L/usr/local/lib -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio -lopencv_face -lopencv_objdetect
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
    DataGeneratorEmbeddedWidget.ui \
    ErodeAndDilateEmbeddedWidget.ui \
    FaceDetectionEmbeddedWidget.ui \
    FloodFillEmbeddedWidget.ui \
    ImageROIEmbeddedWidget.ui \
    RGBsetValueEmbeddedWidget.ui \
    SobelAndScharrEmbeddedWidget.ui \
    SyncGateEmbeddedWidget.ui \
    TemplateEmbeddedWidget.ui

