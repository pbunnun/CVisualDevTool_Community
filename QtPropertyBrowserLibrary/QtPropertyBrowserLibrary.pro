QT += widgets

TEMPLATE = lib

CONFIG += c++14
INCLUDEPATH +=

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QTPROPERTYBROWSER_LIBRARY

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(qtpropertybrowser/qtpropertybrowser.pri)

HEADERS += \
    QtPropertyBrowserLibrary.hpp

win32:CONFIG(release, debug|release): LIBS +=
else:win32:CONFIG(debug, debug|release): LIBS +=
else:unix: LIBS +=

macx: DESTDIR = ../CVisualDev/CVisualDev.app/Contents/MacOS/

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
