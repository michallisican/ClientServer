QT     += core
QT     += network
QT     -= gui

TARGET = ClientServer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    FileOperations.cpp \
    main.cpp \
    ClientWin.cpp \
    ServerWin.cpp

HEADERS += \
    FileOperations.h \
    ClientWin.h \
    ServerWin.h \
    Log.h

DISTFILES +=

