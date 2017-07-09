QT += core
QT -= gui

CONFIG += c++11

TARGET = SendH264ToVLCWithSocket
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

LIBS += -lWs2_32

