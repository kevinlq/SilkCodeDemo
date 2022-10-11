QT = core gui widgets

CONFIG += c++17
TARGET = SilkCode
DESTDIR = $$PWD/bin

INCLUDEPATH += $$PWD/3rdparty/silk
INCLUDEPATH += $$PWD/src

include($$PWD/3rdparty/silk/silk.pri)

HEADERS += \
    $$PWD/src/SilkAudioCode.h \
    MainWidget.h

SOURCES += \
    $$PWD/src/SilkAudioCode.cpp \
    $$PWD/main.cpp \
    MainWidget.cpp
