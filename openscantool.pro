#-------------------------------------------------
#
# Project created by QtCreator 2011-12-02T19:34:07
#
#-------------------------------------------------

QT += core gui
TARGET = openscantool
TEMPLATE = app
CONFIG += link_prl

SOURCES += main.cpp\
        mainwindow.cpp \
    mainscreen.cpp \
    dashboard.cpp \
    gauges.cpp

HEADERS  += mainwindow.h \
    mainscreen.h \
    dashboard.h \
    gauges.h

OTHER_FILES += build_deps.sh

qextserialport.target = qextserialport
qextserialport.commands = ./build_deps.sh qextserialport

LIBS += -Ldeps/qextserialport -lqextserialport -lqwt
INCLUDEPATH += /usr/include/qwt deps/qextserialport
DEPENDPATH += deps/qextserialport
QMAKE_EXTRA_TARGETS += qextserialport
PRE_TARGETDEPS += qextserialport
