QT       += core
QT       -= gui

TARGET = QtPhysFS
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += $$PWD/3rdparty/physfs/include
LIBS += -L$$PWD/3rdparty/physfs/lib -lphysfs

TEMPLATE = app

SOURCES += main.cpp \
    FileEngine.cpp

HEADERS += \
    FileEngine.h
