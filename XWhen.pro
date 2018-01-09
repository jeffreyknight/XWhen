#-------------------------------------------------
#
# Project created by QtCreator 2017-12-22T14:55:28
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XWhen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    openssl.cpp \
    dbmanager.cpp \
    dayplan.cpp \
    monthplan.cpp

HEADERS  += mainwindow.h \
    openssl.h \
    dbmanager.h \
    dayplan.h \
    monthplan.h

FORMS    += mainwindow.ui \
    dayplan.ui

DISTFILES += \
    garbage

QMAKE_CXXFLAGS += -std=c++11

RC_ICONS = xwhen.ico

unix:contains(QMAKE_HOST.arch, x86_64):{
    message("Target is Linux x86_64 64bit")
LIBS += -L"/usr/lib/x86_64-linux-gnu/" -lcrypto -lssl -lsqlite3 -lz
}

win32-g++:{
    message("Target is Windows")
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
    LIBS += -lcrypto -lcrypt32 -lgdi32 -lsqlite3 -lz
}

RESOURCES += \
    xwhen.qrc
