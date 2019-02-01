#include "procmonitor.h"

procMonitor::procMonitor(){
}

procMonitor::~procMonitor(){
}

void procMonitor::update(){
	processes.resize(0);
	std::filesystem::directory_iterator procDir("/proc");
	for(auto proc: procDir){
		std::string path(proc.path().c_str());
		std::string pid(proc.path().filename().c_str());

		if(pid[0] >= '0' && pid[0] <= '9'){
			std::ifstream cmdline(path+"/cmdline");
			std::string programPath;
			programPath.resize(PATH_MAX);
			/*
			* for some reason seeking to the end to get the size doesnt work. im sure its some weird rule
			* or something i dont know about, maybe /proc/<pid> arent real files. they probably arent
			*/
			cmdline.read(programPath.data(), static_cast<std::streamsize>(programPath.size()));
			programPath.resize(static_cast<size_t>(cmdline.gcount()));
			if(programPath.size() == 0){
				continue;
			}

			processes.emplace_back(programPath, std::stoi(pid));
		}
	}
}

int procMonitor::getVibrance(std::vector<programEntry> &watchList){
	int vibrance = defaultVibrance;
	for(auto &program: watchList){
		for(auto &process: processes){
			if(process == program.path){
				vibrance = program.vibrance;
			}
		}
	}
	return vibrance;
}
