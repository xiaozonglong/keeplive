
QT += network
include($$PWD/../qt-solutions/qtservice/src/qtservice.pri)
include($$PWD/../qt-solutions/qtsingleapplication/src/qtsinglecoreapplication.pri)
#DEFINES += MACRO_LOCKFILE
contains(DEFINES,MACRO_LOCKFILE) {
  include($$PWD/../qt-solutions/qtlockedfile/src/qtlockedfile.pri)
}

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

win32{
    LIBS += \
        -lWtsApi32 \
        -lAdvApi32 \
        -lUserEnv
}
