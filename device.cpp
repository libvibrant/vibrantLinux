#include "device.h"

void findDevices(std::vector<device> &devices){
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
			device dev;
			dev.vendor = gpuVendor::nvidia;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}

		if(driver.find("i915") != std::string::npos){
			device dev;
			dev.vendor = gpuVendor::intel;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}

		if(driver.find("amd") != std::string::npos){
			device dev;
			dev.vendor = gpuVendor::amd;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}
	}
}

void findDevicesNvidia(std::vector<device> &devices){
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
			device dev;
			dev.vendor = gpuVendor::nvidia;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}
	}
}

void findDevicesAmd(std::vector<device> &devices){
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
			device dev;
			dev.vendor = gpuVendor::amd;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}
	}
}

void findDevicesIntel(std::vector<device> &devices){
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
			device dev;
			dev.vendor = gpuVendor::intel;
			dev.deviceId = i;
			devices.push_back(dev);
			continue;
		}
	}
}
