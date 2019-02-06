#include "procmonitor.h"

procMonitor::procMonitor(){
}

procMonitor::~procMonitor(){
}

void procMonitor::update(){
	processes.resize(0);

	QDir procDir("/proc");
	QStringList procs = procDir.entryList(QDir::Filter::AllDirs);
	for(auto p: procs){
		std::string proc = p.toStdString();
		if(proc[0] >= '0' && proc[0] <= '9'){
			std::ifstream cmdline("/proc/"+proc+"/cmdline");
			std::string programPath;
			programPath.resize(PATH_MAX);
			/*
			* for some reason seeking to the end to get the size doesnt work. im sure its some weird rule
			* or something i dont know about, maybe /proc/<pid> arent real files. they probably arent
			*/
			cmdline.read(&programPath[0], static_cast<std::streamsize>(programPath.size()));
			programPath.resize(static_cast<size_t>(cmdline.gcount()));
			if(programPath.size() == 0){
				continue;
			}

			processes.emplace_back(programPath, std::stoi(proc));
		}
	}
}

QListWidgetItem *procMonitor::getVibrance(QListWidget *&watchList){
	QListWidgetItem *res = nullptr;
	for(int i = 0; i < watchList->count(); i++){
		for(auto &process: processes){
			if(process == itemPath(watchList->item(i))){
				res = watchList->item(i);
			}
		}
	}
	return res;
}
