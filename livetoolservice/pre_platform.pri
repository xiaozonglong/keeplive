#QT       +=core gui network

CONFIG += warn_off
CONFIG -= console
TEMPLATE = app
#QMAKE_CXXFLAGS += -fexceptions
#QMAKE_CXXFLAGS += -Wno-unused-parameter
#QMAKE_CFLAGS += -fexceptions
CONFIG += exceptions
#CONFIG += staticdll
CONFIG += attachdll
CONFIG  -= exceptions_off
QMAKE_CFLAGS += -std=c99

#CONFIG += DEBUG



static{
#    DEFINES += QT_NO_DEBUG_OUTPUT
    DEFINES -= MACRO_SHARED
    DEFINES += MACRO_STATIC
    libstype = static
}
shared {
    DEFINES -= MACRO_STATIC
    DEFINES += MACRO_SHARED
    libstype = shared
}
CONFIG -= debug_and_release debug_and_release_target
CONFIG(release, debug|release){
        !contains(DEFINES,MACRO_RELEASE) {
        DEFINES -= MACRO_DEBUG
        DEFINES += MACRO_RELEASE
        modetype = release
        CONFIG -= debug
    }
}else{
    !contains(DEFINES,MACRO_DEBUG) {
        DEFINES -= MACRO_RELEASE
        DEFINES += MACRO_DEBUG
        modetype = debug
         CONFIG -= release
        unix:TARGET = $$TARGET"d"

    }
}

message("$$libstype $$modetype build.$$DEFINES ")



contains(TEMPLATE, ".*app"){
    QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS
    win32-g++:DEFINES += QT_NEEDS_QMAIN
    win32-borland:DEFINES += QT_NEEDS_QMAIN

    qt:for(entryLib, $$list($$unique(QMAKE_LIBS_QT_ENTRY))) {
        isEqual(entryLib, -lqtmain): {
            CONFIG(debug, debug|release): QMAKE_LIBS += $${entryLib}$${QT_LIBINFIX}d
            else: QMAKE_LIBS += $${entryLib}$${QT_LIBINFIX}
        } else {
            QMAKE_LIBS += $${entryLib}
        }
    }
}



contains(QT_ARCH, i386): ARCHITECTURE = x86
else: ARCHITECTURE = $$QT_ARCH

macx: PLATFORM = "mac"
else:win32: PLATFORM = "windows"
else:linux-*: PLATFORM = "linux-$${ARCHITECTURE}"
else:android:PLATFORM = "android"
else: PLATFORM = "unknown"


message("PLATFORM="$$PLATFORM)

#arm开发板
contains(PLATFORM,linux-arm64){
    CONFIG += linux_aarch64
    linux_aarch64{

      DEFINES += MACRO_linux_aarch4

      CONFIG += HTNICE

      HTNICE{

              DEFINES += MACRO_HTNICE_GT765X
              message("MACRO_HTNICE_GT765X")

      }
    }
}
contains(PLATFORM,linux-arm){
    CONFIG += linux_armv5
#    CONFIG += linux_armv7

#
     DEFINES += MACRO_linux_arm
    linux_armv7{
        DEFINES += MACRO_linux_armv7
#        CONFIG += MCAM335x
        CONFIG += GENERAL
#        CONFIG += HTNICE
#        CONFIG += HT-C22
        MCAM335x{
                #此硬件参考MC-AM335x-IOT BOXV6
                DEFINES += MACRO_IOTV6
                message("MACRO_MCAM335x")
                #此硬件参考MC-AM335x-B813
#                DEFINES += MACRO_B813
                #此硬件参考MC-AM335x-Lite V2.3
#                DEFINES += MACRO_Lite_2_3

#                DEFINES += MACRO_MCAM335x
#                message("MACRO_MCAM335x")
                #linux IIC字符设备
        }
        HTNICE{

                DEFINES += MACRO_HTNICE_GT665X
                message("MACRO_HTNICE_GT665X")
                #linux IIC字符设备
        }
        HT-C22{

                DEFINES += MACRO_HT-C22
                message("MACRO_HT-C22")
                #linux IIC字符设备
        }
        GENERAL{
                DEFINES += MACRO_GENERAL
                message("GENERAL Device")
                #linux IIC字符设备
        }
      }
      linux_armv5{

        DEFINES += MACRO_linux_armv5
#        CONFIG += MCAM335x
        CONFIG += HTNICE

        DEFINES += QT_NO_SSL
        HTNICE{

                DEFINES += MACRO_HTNICE_GT6502
                message("MACRO_HTNICE_GT6502")

        }
      }
    }

#linux_mips开发板
contains(PLATFORM,linux-mips){
    CONFIG += linux_mips
    linux_mips{
        DEFINES += MACRO_linux_mipsle
        CONFIG += OPENWRT
        OPENWRT{

        }
    }
}

#linux操作系统
contains(PLATFORM,linux-x86_64){

    CONFIG += linux_x86_64
    linux_x86_64{
        DEFINES += MACRO_linux_x86_64
        DEFINES += QT_NO_SSL
#        message("$$PLATFORM QT_NO_SSL-QT_NO_SSL--QT_NO_SSL----------------------")
    }
}

contains(PLATFORM,linux-arm64){
    CONFIG += linux_arm64
    linux_arm64{
        DEFINES += MACRO_linux_arm64
        DEFINES += QT_NO_SSL
#        message("$$PLATFORM QT_NO_SSL-QT_NO_SSL--QT_NO_SSL----------------------")
    }
}
#windows
contains(PLATFORM,windows){
    CONFIG += windows

}

#android
contains(PLATFORM,android){
    CONFIG += android
    QT       += androidextras
    windows{
            DEFINES += MACRO_android

    }
}


QMAKE_XSPEC = $$[QMAKE_XSPEC]
message("QMAKE_XSPEC----------------------"$$[QMAKE_XSPEC])


contains(QMAKE_XSPEC,"win32-msvc"){
    message("-xxxxxxxxx----------------------"$$[QT_VERSION]  $$QMAKE_XSPEC)
    CONFIG += WIN32MSVC
    CONFIG += WINMSVC
    CONFIG += no_batch
    DEFINES += MACRO_MSVC
    exists($$[QT_INSTALL_LIBS]/vld.lib){
        exists($$[QT_INSTALL_HEADERS]/vld.h){
            DEFINES += MACRO_VLD
            LIBS += -L$$[QT_INSTALL_LIBS] -lvld
        }
    }
    #win32:DEFINES += _WINSOCKAPI_
    DEPENDPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.17763.0\km\crt"
}
contains(QMAKE_XSPEC,win32-g++){

    CONFIG += WIN32GCC
    DEFINES += MACRO_GCC
}

contains(QMAKE_XSPEC,linux.*){

    CONFIG += LINUXGCC
    DEFINES += MACRO_linux_GCC
}

contains(QMAKE_XSPEC,android-g++){

   CONFIG += ANDROID_GCC
    DEFINES += MACRO_ANDROID_GCC
}

message("xxxxxx---------------------"$$[QT_VERSION]  $$QMAKE_XSPEC)
# 版本信息
VERSION = $$[QT_VERSION]
# 编译链信息


contains(QMAKE_XSPEC,"win32-msvc.*"){
#    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'uiAccess='false'"
    QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
}

#加入权限
win32{
#以管理员运行
#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#QMAKE_LFLAGS += /SUBSYSTEM:WINDOWS,\"5.01\" #VS2013 在XP运行
}
#add my LIB
#Python
debug{
#    include($$PWD/python.pri)
}
