#-------------------------------------------------
#
# Project created by QtCreator 2016-08-23T11:17:30
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simulador_TCP
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    smartplugconnection.cpp

HEADERS  += mainwindow.h \
    smartplugconnection.h

FORMS    += mainwindow.ui
