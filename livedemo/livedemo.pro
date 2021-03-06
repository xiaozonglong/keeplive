#-------------------------------------------------
#
# Project created by QtCreator 2019-02-17T20:23:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = livedemo
TEMPLATE    = app
MOC_DIR     = temp/moc
RCC_DIR     = temp/rcc
UI_DIR      = temp/ui
OBJECTS_DIR = temp/obj
DESTDIR     = $$PWD/../bin
CONFIG      += warn_off

SOURCES     += main.cpp
SOURCES     += frmmain.cpp
HEADERS     += frmmain.h
FORMS       += frmmain.ui

include($$PWD/appkeeplive/appkeeplive.pri)

include($$PWD/../livetoolservice/qt-solutions/qtlockedfile/src/qtlockedfile.pri)
