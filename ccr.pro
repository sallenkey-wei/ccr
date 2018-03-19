#-------------------------------------------------
#
# Project created by QtCreator 2018-02-07T13:23:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ccr
TEMPLATE = app

INCLUDEPATH += D:/opencv/opencv3.1.0/build/include
LIBS += -LD:/opencv/opencv3.1.0/build/x64/vc14/lib -lopencv_world310

SOURCES += main.cpp\
        mainwindow.cpp \
    camera.cpp \
    log.cpp \
    mymessagebox.cpp \
    transactionthread.cpp \
    processmgmt.cpp \
    configdialog.cpp

HEADERS  += mainwindow.h \
    camera.h \
    include/image_select.h \
    common.h \
    log.h \
    mymessagebox.h \
    transactionthread.h \
    processmgmt.h \
    configdialog.h

RESOURCES += \
    res.qrc

FORMS += \
    groupboxforlocation.ui \
    groupboxforonline.ui \
    mainwindow.ui \
    widgetforopensource.ui \
    widgetforsavesource.ui \
    configdialog.ui

#win32: LIBS += -L$$PWD/lib/ -lcontainer_select_dll

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
