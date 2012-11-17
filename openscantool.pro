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

libobd.target = libobd
libobd.commands = ./build_deps.sh libobd

LIBS += -Ldeps/libobd -lobd -lqwt
INCLUDEPATH += /usr/include/qwt deps/libobd
DEPENDPATH += deps/libobd
QMAKE_EXTRA_TARGETS += libobd
PRE_TARGETDEPS += libobd
