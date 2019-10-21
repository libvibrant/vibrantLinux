#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[]){
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
