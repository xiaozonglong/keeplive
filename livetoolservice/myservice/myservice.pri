
QT += network
include($$PWD/../qt-solutions/qtservice/src/qtservice.pri)
include($$PWD/../qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
include($$PWD/../qt-solutions/qtlockedfile/src/qtlockedfile.pri)
DEFINES += _UNICODE
DEFINES += MACRO_MYSERVICR
include($$PWD/plugins/plugins.pri)
SOURCES += \
    $$PWD/keeplive.cpp \
    $$PWD/myservice.cpp



HEADERS  += \
    $$PWD/keeplive.h \
    $$PWD/myservice.h

LIBS += \
    -L$$DESTDIR \
    -lWtsApi32 \
    -lAdvApi32 \
    -lUserEnv
