QT     += core
QT     += network
QT     -= gui

TARGET = ClientServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    ../Source/ClientWin.cpp \
    ../Source/FileOperations.cpp \
    ../Source/main.cpp \
    ../Source/ServerWin.cpp

HEADERS += \
    ../Source/ClientWin.h \
    ../Source/FileOperations.h \
    ../Source/Log.h \
    ../Source/ServerWin.h

DISTFILES +=

