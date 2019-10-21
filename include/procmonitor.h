#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <vector>
#include <fstream>

#include <unistd.h>
#include <linux/limits.h>

#include <QString>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QListWidgetItem>

#include "displaytab.h"


/*
* we could use the proc connector... but no
* it would require sudo and be really annoying to work with
*/


struct process{
	std::string path;
	pid_t pid;

	process(){}
	process(std::string path, pid_t pid) : path(path), pid(pid){}
	process(const process &other) : path(other.path), pid(other.pid){}

	bool operator==(std::string path){
		return this->path.find(path) != std::string::npos;
	}
	bool operator==(QString path){
		return this->path.find(std::string(path.toUtf8())) != std::string::npos;
	}
};

class procMonitor{
public:
	procMonitor();
	~procMonitor();

	void update();
	//if this returns defaultVibrance it means no programs in the watch list are running
	QListWidgetItem *getVibrance(QListWidget *&watchList);

private:
	std::vector<process> processes;
};

#endif // PROCMONITOR_H
