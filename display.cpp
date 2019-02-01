#include "display.h"
#include <iostream>

display::display(){
}

display::display(displayType type, std::string displayName) : type(type), displayName(displayName){
}

display::display(const display &other) noexcept{
	type = other.type;
	displayName = other.displayName;
}

display::display(display &&other) noexcept{
	type = other.type;
	displayName = std::move(other.displayName);
}

display::~display(){
}

void display::applyVibrance(int vibrance){
	if(type == displayType::nvidia){
		std::string sysCall("nvidia-settings -a \"[" + displayName + "]");
		sysCall += "/DigitalVibrance=" + std::to_string(vibrance) + "\"";
		system(sysCall.c_str());
	}
}

std::vector<display> display::getDisplays(){
	std::vector<display> displays;
	if(systemHasNvidia()){
		QProcess nvidia;
		QProcess grep;
		nvidia.setStandardOutputProcess(&grep);
		nvidia.start("nvidia-settings -q dpys");
		grep.start("grep connected");
		grep.setProcessChannelMode(QProcess::ForwardedChannels);
		nvidia.waitForFinished();
		grep.waitForFinished();

		/*
		* The output should be in this format
		* [0] HOSTNAME:1[dpy:0] (DVI-D-0) (connected, enabled)
		*
		* note that the extra new line above was left on purpose
		* nvidia puts an extra new line at the end of the command
		*/
		QStringList res = QString(grep.readAll()).split("\n");

		for(int i = 0; i < res.size() - 1; i++){
			//im sorry i know this looks horrendous
			std::string displayName = std::string(res[i].split("(")[1].split(")")[0].toUtf8());
			std::cout << displayName << std::endl;

			displays.emplace_back(displayType::nvidia, displayName);
		}
	}
	return displays;
}
