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
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

INCLUDEPATH += "include/"

!contains(DEFINES, VIBRANT_LINUX_NO_XCB){
LIBS += \
	-lxcb \
	-lxcb-ewmh
}

SOURCES += \
	src/displaytab.cpp \
	src/entryeditor.cpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/procscanner.cpp \
	src/utils.cpp

HEADERS += \
	include/displaytab.h \
	include/entryeditor.h \
	include/mainwindow.h \
	include/procscanner.h \
	include/programinfo.h \
	include/utils.h

FORMS += \
	forms/entryeditor.ui \
	forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
