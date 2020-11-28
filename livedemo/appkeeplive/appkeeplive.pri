INCLUDEPATH += $$PWD

DEFINES += MACRO_APP_KEEPLIVE
contains(DEFINES,MACRO_APP_KEEPLIVE){
    #入口


    SOURCES += \
        $$PWD/appkeeplive.cpp

    HEADERS += \
        $$PWD/appkeeplive.h

}


