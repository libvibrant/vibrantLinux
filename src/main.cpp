#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>

#include <unistd.h>
#include <iostream>

#include "mainwindow.h"

int main(int argc, char *argv[]){
	//don't run if the program is already running
	QString vlPath = QFileInfo("/proc/"+QString::number(getpid())+"/exe").canonicalFilePath();

	QDir procDir("/proc");
	QStringList procs = procDir.entryList(QDir::Filter::Dirs);
	procs = procs.filter(QRegularExpression("^\\d+$"));
	procs.removeOne(QString::number(getpid()));

	for(auto &proc: procs){
		if(vlPath == QFileInfo("/proc/"+proc+"/exe").canonicalFilePath()){
			std::cout << "Vibrant Linux is already running" << std::endl;
			return EXIT_SUCCESS;
		}
	}

	QApplication a(argc, argv);
	QCommandLineParser parser;
	parser.addOption(QCommandLineOption("hidden"));
	parser.parse(QCoreApplication::arguments());

	mainWindow w;
	if(parser.isSet(("hidden"))){
		w.systray.show();
		w.hide();
	}
	else{
		w.show();
	}

	return a.exec();
}
