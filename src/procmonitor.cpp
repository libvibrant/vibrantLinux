#include "procmonitor.h"

procMonitor::procMonitor(){
}

procMonitor::~procMonitor(){
}

QListWidgetItem* procMonitor::getVibrance(QListWidget *&watchList){
	QListWidgetItem *res = nullptr;
	updateProcessesVector();

	//traverse in reverse. First items get priority in vibrance control
	for(int i = watchList->count()-1; i >= 0; i--){
		QListWidgetItem *item = watchList->item(i);
		for(auto &process: processes){
			if(process == getItemPath(item)){
				res = watchList->item(i);
			}
		}
	}
	return res;
}

void procMonitor::updateProcessesVector(){
	processes.resize(0);

	QDir procDir("/proc");
	QStringList procs = procDir.entryList(QDir::Filter::AllDirs);
	for(auto proc: procs){
		//only program folders have numbers
		if(proc[0] >= '0' && proc[0] <= '9'){
			QFileInfo procInfo("/proc/"+proc+"/exe");
			processes.emplace_back(procInfo.canonicalFilePath());
		}
	}
}
