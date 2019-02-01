#include "device.h"

bool systemHasNvidia(){
	for(size_t i = 0;; i++){
		char *realPath = realpath(std::string("/sys/class/drm/card"+std::to_string(i)+"/device/driver").c_str(), nullptr);
		if(!realPath){
			break;
		}
		std::string driver(realPath);
		free(realPath);

		for(char &c: driver){
			c = static_cast<char>(std::tolower(c));
		}

		if(driver.find("nvidia") != std::string::npos){
			return true;
		}
	}
	return false;
}

bool systemHasAmd(){
	for(size_t i = 0;; i++){
		char *realPath = realpath(std::string("/sys/class/drm/card"+std::to_string(i)+"/device/driver").c_str(), nullptr);
		if(!realPath){
			break;
		}
		std::string driver(realPath);
		free(realPath);

		for(char &c: driver){
			c = static_cast<char>(std::tolower(c));
		}

		if(driver.find("amd") != std::string::npos){
			return true;
		}
	}
	return false;
}

bool systemHasIntel(){
	for(size_t i = 0;; i++){
		char *realPath = realpath(std::string("/sys/class/drm/card"+std::to_string(i)+"/device/driver").c_str(), nullptr);
		if(!realPath){
			break;
		}
		std::string driver(realPath);
		free(realPath);

		for(char &c: driver){
			c = static_cast<char>(std::tolower(c));
		}

		if(driver.find("i915") != std::string::npos){
			return true;
		}
	}
	return false;
}
