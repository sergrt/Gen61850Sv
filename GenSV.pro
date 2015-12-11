#-------------------------------------------------
#
# Project created by QtCreator 2013-05-13T16:17:56
#
#-------------------------------------------------

QT += core gui network testlib widgets printsupport

#VERSION = 1.5.0a
#DEFINES += VERSION=\\\"$$VERSION\\\"

#CONFIG += testlib debug
QMAKE_CXXFLAGS += -O2 #-Wno-write-strings -std=gnu++0x

TARGET = GenSv

win32: INCLUDEPATH += WinPCap/Include \
    qwt/src
linux: INCLUDEPATH += qwt/src

#TEMPLATE = app


SOURCES += \
    capture.cpp \
    chart.cpp \
    editwnd.cpp \
    EthernetHeader.cpp \
    generator.cpp \
    genParams.cpp \
    helpers.cpp \
    main.cpp \
    mainwindow.cpp \
    PlotCurve.cpp \
    scpi.cpp \
    settings.cpp \
    ShapeDefine.cpp \
    ShapeDisplay.cpp \
    splash.cpp \
    SVPacket.cpp \
    SyncObject.cpp \
    TcpServer.cpp \
    TLV.cpp \
    VirtualKeyboard.cpp

HEADERS  += \
    capture.h \
    chart.h \
    editwnd.h \
    EthernetHeader.h \
    generator.h \
    genParams.h \
    helpers.h \
    mainwindow.h \
    PlotCurve.h \
    scpi.h \
    settings.h \
    ShapeDefine.h \
    ShapeDisplay.h \
    splash.h \
    SVDecoded.h \
    SVPacket.h \
    SyncObject.h \
    TcpServer.h \
    TLV.h \
    VirtualKeyboard.h


FORMS    += mainwindow.ui \
    editwnd.ui \
    VirtualKeyboard.ui \
    ShapeDisplay.ui \
    ShapeDefine.ui \
    splash.ui

win32: LIBS += -L/work_projects/GenSv/WinPCap/Lib -lwpcap \
    -L/work_projects/GenSv/qwt/Lib -lqwtd
linux: LIBS += -lpcap -L~/work_projects/GenSv/trunk/qwt/lib -lqwt

TRANSLATIONS   = GenSV_en.ts \
GenSV_ru.ts

RESOURCES = GenSv.qrc
