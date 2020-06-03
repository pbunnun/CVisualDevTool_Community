QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS NODE_EDITOR_SHARED

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    MainWindow.hpp

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../NodeEditor/release/ -lNodeEditor -L$$PWD/../CVisualDevLibrary/release/ -lCVisualDevLibrary \
    -L$$PWD/../QtPropertyBrowserLibrary/release/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world420 -LC:\opencv\build\x64\vc15\bin
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../NodeEditor/debug/ -lNodeEditor -L$$PWD/../CVisualDevLibrary/debug/ -lCVisualDevLibrary\
    -L$$PWD/../QtPropertyBrowserLibrary/debug/ -lQtPropertyBrowserLibrary -LC:\opencv\build\x64\vc15\lib -lopencv_world420d -LC:\opencv\build\x64\vc15\bin
else:unix:!macx {
        LIBS += -L$$PWD/../NodeEditor/ -lNodeEditor -L$$PWD/../CVisualDevLibrary -lCVisualDevLibrary -L$$PWD/../QtPropertyBrowserLibrary -lQtPropertyBrowserLibrary -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }
else:macx {
        LIBS += -L../CVisualDev/CVisualDev.app/Contents/MacOS/ -lNodeEditor -lCVisualDevLibrary -lQtPropertyBrowserLibrary -L/usr/local/lib -lopencv_core -lopencv_imgcodecs \
        -lopencv_imgproc -lopencv_videoio
    }

INCLUDEPATH += $$PWD/../NodeEditor/include $$PWD/../NodeEditor/include/nodes $$PWD/../CVisualDevLibrary $$PWD/../QtPropertyBrowserLibrary $$PWD/../QtPropertyBrowserLibrary/qtpropertybrowser
DEPENDPATH += $$PWD/../NodeEditor

win32: INCLUDEPATH += C:\opencv\build\include
