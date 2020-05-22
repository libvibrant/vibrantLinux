#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T11:36:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vibrantLinux
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
		   VIBRANT_LINUX_VERSION=\\\"2.0.1\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

INCLUDEPATH += "include/"

LIBS += \
	-lxcb \
	-lxcb-ewmh \
	-lX11 \
	-lXrandr \
	-lXNVCtrl \
	-lvibrant

SOURCES += \
	src/ctmcontroller.cpp \
	src/displaymanager.cpp \
	src/displaytab.cpp \
	src/entryeditor.cpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/nvidiacontroller.cpp \
	src/procscanner.cpp

HEADERS += \
	include/ctmcontroller.h \
	include/displaycontroller.h \
	include/displaymanager.h \
	include/displaytab.h \
	include/entryeditor.h \
	include/mainwindow.h \
	include/nvidiacontroller.h \
	include/procscanner.h \
	include/programinfo.h

FORMS += \
	forms/entryeditor.ui \
	forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resources.qrc
