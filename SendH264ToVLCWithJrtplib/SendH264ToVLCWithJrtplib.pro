QT += core
QT -= gui

CONFIG += c++11
DEFINES += WIN32

TARGET = SendH264ToVLCWithJrtplib
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

HEADERS += \
    h264.h

LIBS += -lWs2_32

LIBS += $$PWD/lib/libjrtplib.dll.a\

INCLUDEPATH += $$PWD/include/jrtplib\
               $$PWD/include/jthread\
               $$PWD/include
