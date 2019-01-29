#ifndef DEVICE_H
#define DEVICE_H

#include <unistd.h>
#include <string>
#include <vector>

enum class gpuVendor{
	nvidia,
	amd,
	intel,
	unkown
};

struct device{
	gpuVendor vendor = gpuVendor::unkown;
	size_t deviceId;
};

void findDevices(std::vector<device> &devices);
void findDevicesNvidia(std::vector<device> &devices);
void findDevicesAmd(std::vector<device> &devices);
void findDevicesIntel(std::vector<device> &devices);

#endif // DEVICE_H
