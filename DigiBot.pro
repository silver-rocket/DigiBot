#-------------------------------------------------
#
# Project created by QtCreator 2015-05-28T01:21:20
#
#-------------------------------------------------

QT       += core gui
QT       += webkit webkitwidgets
QT       += network

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DigiBot
TEMPLATE = app


SOURCES += main.cpp\
    ircmsgparser.cpp \
    widget.cpp \
    simplexmlreadwrite.cpp \
    twitchapi.cpp \
    chatengine.cpp \
    botengine.cpp \
    random.cpp \
    settings.cpp


HEADERS  += widget.h \
    ircmsgparser.h \
    simplexmlreadwrite.h \
    twitchapi.h \
    chatengine.h \
    botengine.h \
    random.h \
    settings.h

FORMS    += widget.ui

RESOURCES += \
    resources.qrc
