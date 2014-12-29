#-------------------------------------------------
#
# Project created by QtCreator 2014-07-29T00:30:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport  printsupport

TARGET = Qt_Serial_GUI_test_02
TEMPLATE = app
INCLUDEPATH += .
SOURCES += main.cpp\
    qcustomplot.cpp \
    settingsdialog.cpp \
    mainwindow.cpp

HEADERS  += \
    qcustomplot.h \
    settingsdialog.h \
    mainwindow.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    file.qrc
