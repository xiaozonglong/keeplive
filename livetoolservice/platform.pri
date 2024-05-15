
win32{
# 公司名称
QMAKE_TARGET_COMPANY = "Lchtime"

# 产品名称
QMAKE_TARGET_PRODUCT = "Qt Creator"

# 文件说明
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on  $$(VERSION)"

# 版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright 2008-2016 The Lchtime Ltd. All rights reserved."

# 中文（简体）
RC_LANG = 0x0004
}
win32{
CONFIG(debug, debug|release){
#JQToolsmasterPath = $$OUT_PWD/../../../JQTools-master
#JQToolsmasterSourcePath = $$PWD/../../../JQTools-master
#include( $$JQToolsmasterPath/library/JQToolsLibrary/JQToolsLibrary.pri )
#include( $$JQToolsmasterPath/library/JQNetwork/JQNetwork.pri )
#include( $$JQToolsmasterPath/library/MaterialUI/MaterialUI.pri )
#include( $$JQToolsmasterPath/components/components.pri )
}
}

#lib
static{
    minQtVersion(5,0,0) {
        message("加载 static"$$(QTDIR))
        unix{
#        QTPLUGIN += qico
  #      QTPLUGIN += qjpeg4 qgif4

        #QTPLUGIN += qvnc
        LIBS += -L$$(QTDIR)/plugins/platforms/ -lqvnc -lqminimal
        LIBS += $$(QTDIR)/plugins/imageformats/libqico.a

        LIBS += $$(QTDIR)/plugins/sqldrivers/libqsqlite.a

        LIBS += $$(QTDIR)/plugins/platforms/libqlinuxfb.a
#        LIBS += $$(QTDIR)/plugins/platforms/libqvnc.a
        QTPLUGIN += qminimal
        QTPLUGIN += qvnc
        LIBS += $$(QTDIR)/plugins/imageformats/libqgif.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqicns.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqjpeg.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqico.a

        LIBS += $$(QTDIR)/plugins/imageformats/libqtga.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqtiff.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqwbmp.a
        LIBS += $$(QTDIR)/plugins/imageformats/libqwebp.a
        LIBS += $$(QTDIR)/lib/libqtlibpng.a
#        LIBS += $$(QTDIR)/lib/cmake/Qt5Network/Qt5Network_QConnmanEnginePlugin.cmake
#        LIBS += $$(QTDIR)/plugins/iconengines/libqsvgicon.a
        }
    }else{
    win32{
        QTPLUGIN += qico qcncodecs
#        QTPLUGIN += qjpeg4 qgif4
        LIBS += $$(QTDIR)/plugins/imageformats/libqico.a
#        LIBS += $$(QTDIR)/plugins/imageformats/libqjpeg4.a
#        LIBS += $$(QTDIR)/plugins/imageformats/libqgif4.a
#        LIBS += $$(QTDIR)/plugins/sqldrivers/libqsqlite4.a
    }

    }
    message("QT Static build.include plugins has : $$QTPLUGIN")
}
#
contains(PLATFORM,linux-arm){
    minQtVersion(5,0,0) {
        message("加载 qtlibpng")
#    qtHaveModule(qtlibpng) {
        DEFINES += MACRO_PNG
        LIBS += -L$$(QTDIR)/lib/ -lqtlibpng
#        }
    }else{
        message("移除 qtlibpng")
    }
}else{

}
PRETARGET = $$TARGET

#TARGET = $$qtLibraryTarget($$TARGET)
CONFIG(debug, debug|release) {
    win32: TARGET = $$join(TARGET,,,d)
    mac: TARGET = $$join(TARGET,,,_debug)
    unix:!mac: TARGET = $$join(TARGET,,,d)
}

#定义相对路径库
#LIBS += -L ./3rdparty  \
#        -L ./lib/  \


#!equals(_PRO_FILE_PWD_, OUT_PWD) {
##do something when using shadow build
#message("do something when using shadow build"$$OUT_PWD)
#}else{

#message("do something when no using shadow build")
#}
#_PRO_FILE_PWD_ =$$target_path/$$PLATFORM/$$[QT_VERSION]/$$QMAKE_XSPEC/out
#OUT_PWD =       $$target_path/$$PLATFORM/$$[QT_VERSION]/$$QMAKE_XSPEC/out
PRO_NAME = $$relative_path($$IDE_SOURCE_TREE, $$IDE_BIN_PATH/../../)
DESTDIR =       ../$$TEMPLATE/$$PLATFORM/$$[QT_VERSION]/$$QMAKE_XSPEC/$$libstype/$$PRO_NAME

# 定义输出路径
target_path = ../build/$$PRETARGET-$$modetype-$$libstype
target_path1 = $$target_path-$$PLATFORM-$$[QT_VERSION]-$$QMAKE_XSPEC
MOC_DIR =       $$target_path1/moc
RCC_DIR =       $$target_path1/rcc
UI_DIR =        $$target_path1/ui
OBJECTS_DIR =   $$target_path1/obj
#    message("``````target_path="$$target_path $$MOC_DIR)

unix{
    android{
        message(android)
        CONFIG += mobility
        MOBILITY =
#        target.files = TARGET
#        target.path = /tmp/
#        INSTALLS += target

    }else{
        #生成开发板调试应用文件
        target.files = $$TARGET

        target.path = /mnt/root
        MCAM335x{
            static{
#                target.path = /mnt/mmc/qt/bin
                target.path = /home/root/qt/bin
#                target.path = /mnt/mmc/qtappstatic
#                  target.path = /opt/qt/bin

            }else{
                target.path = /mnt/mmc/qtapp
            }
        }
        HTNICE{
            static{
                 target.path = /opt/qt/bin
#                 contains(DEFINES,MACRO_HTNICE_GT6502){
#                                target.path = /mnt/udisk/qt/bin
#                }
            }else{
                target.path = /mnt/mmc/qtapp
            }
        }
        HT-C22{
            static{
                 target.path = /root/qt/bin

            }else{
                target.path = /mnt/mmc/qtapp
            }
        }
        GENERAL{
            static{
#                target.path = /mnt/mmc/qt/bin
#                target.path = /home/root/qt/bin
#                target.path = /mnt/mmc/qtappstatic
                  target.path = /opt/qt/bin

            }else{
                target.path = /mnt/mmc/qtapp
            }
        }
        OPENWRT:target.path = /mnt/sda/qt/bin
        INSTALLS += target
        message("指定生成的应用程序放置的目录----"$$DESTDIR/$$TARGET)
        message("target.path----"$$target.path)
        message("target.files----"$$target.files)

    }
}else{
#target.path = ./lib/
#LIBS += -L$$DESTDIR/lib
#message("QT "$$DESTDIR)
}

