INCLUDEPATH += $$PWD


SOURCES += \
    $$PWD/app.cpp \
    $$PWD/utility.cpp \

HEADERS  += \
    $$PWD/app.h \
    $$PWD/utility.h \


linux{
    SOURCES += \
        $$PWD/isyslinux.cpp \
        $$PWD/process_loader.cpp \


    HEADERS  += \
        $$PWD/isyslinux.h \
        $$PWD/process_loader.h \

}

win32{
    SOURCES += \
        $$PWD/isyswin.cpp \
        $$PWD/process_loader.cpp \


    HEADERS  += \
        $$PWD/isysinfo.h \
        $$PWD/isyswin.h \
        $$PWD/process_loader.h \

}
