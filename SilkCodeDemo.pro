QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = SilkCode
DESTDIR = $$PWD/bin

INCLUDEPATH += $$PWD/3rdparty/silk
INCLUDEPATH += $$PWD/src

include($$PWD/3rdparty/silk/silk.pri)

HEADERS += \
    $$PWD/src/SilkAudioCode.h

SOURCES += \
    $$PWD/src/SilkAudioCode.cpp \
    $$PWD/main.cpp
