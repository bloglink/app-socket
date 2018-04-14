#-------------------------------------------------
#
# Project created by QtCreator 2018-04-10T10:28:17
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app-socket
TEMPLATE = app

RC_FILE += qrc/appsource.rc

HEADERS += \
    app/appsocket.h

SOURCES += \
    app/appsocket.cpp \
    app/main.cpp

RESOURCES += \
    qrc/appsource.qrc

