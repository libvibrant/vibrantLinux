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
		   VIBRANT_LINUX_VERSION=\\\"2.1.8\\\"

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
	src/autostart.cpp \
	src/displaymanager.cpp \
	src/displaytab.cpp \
	src/entryeditor.cpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/procscanner.cpp

HEADERS += \
	include/autostart.h \
	include/displaymanager.h \
	include/displaytab.h \
	include/entryeditor.h \
	include/mainwindow.h \
	include/procscanner.h \
	include/programinfo.h

FORMS += \
	forms/entryeditor.ui \
	forms/mainwindow.ui

# get PREFIX from env or fallback to /usr
prefix = $$(PREFIX)
isEmpty(prefix): prefix = "/usr"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = $${prefix}/bin
!isEmpty(target.path): INSTALLS += target


desktopconfig.input = assets/io.github.libvibrant.vibrantLinux.desktop.in
desktopconfig.output = io.github.libvibrant.vibrantLinux.desktop

QMAKE_SUBSTITUTES += \
	desktopconfig

desktop.path = $${prefix}/share/applications
desktop.files += $${desktopconfig.output}

icons.path = $${prefix}/share/icons/hicolor/512x512/apps
icons.files += assets/io.github.libvibrant.vibrantLinux.png

appdata.path = $${prefix}/share/metainfo
appdata.files += assets/io.github.libvibrant.vibrantLinux.appdata.xml

INSTALLS += desktop icons appdata

RESOURCES += \
	resources.qrc

