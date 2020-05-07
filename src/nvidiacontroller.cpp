#include "nvidiacontroller.h"

size_t nvidiaController::instance_count = 0;
Display* nvidiaController::dpy = nullptr;

nvidiaController::nvidiaController(nvidiaDisplay &display): display(display){
	if(instance_count == 0){
		dpy = XOpenDisplay(nullptr);
		if(dpy == nullptr){
			throw std::runtime_error("failed to open connection to X server");
		}
	}
}

nvidiaController::~nvidiaController(){
	instance_count--;
	if(instance_count == 0){
		XCloseDisplay(dpy);
	}
}

int nvidiaController::getSaturation(){
	int saturation;
	XNVCTRLQueryTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_DISPLAY, display.id, 0, NV_CTRL_DIGITAL_VIBRANCE, &saturation);
	if(saturation >= 9){
		return (saturation*100)/1023;
	}
	else{
		return (saturation*100)/1024;
	}
}

void nvidiaController::setSaturation(int saturation){
	if(saturation >= 0){
		saturation = (saturation*1023)/100;
	}
	else{
		saturation = (saturation*1024)/100;
	}
	XNVCTRLSetTargetAttribute(dpy, NV_CTRL_TARGET_TYPE_DISPLAY, display.id, 0, NV_CTRL_DIGITAL_VIBRANCE, saturation);
}

bool nvidiaController::existsOnSystem(Display *dpy){
	int major, minor;
	return XNVCTRLQueryVersion(dpy, &major, &minor);
}

int nvidiaController::getNvScreen(Display *dpy){
	int screen = DefaultScreen(dpy);
	if(XNVCTRLIsNvScreen(dpy, screen)){
		return screen;
	}

	for(int i = 0; i < ScreenCount(dpy); i++){
		if(XNVCTRLIsNvScreen(dpy, i)){
			return screen;
		}
	}

	return -1;
}

std::vector<nvidiaDisplay> nvidiaController::getDisplays(Display *dpy, int screen){
	std::vector<nvidiaDisplay> dpys;
	int *nvDpyIds;
	//length of data returned in bytes
	int nvDpyIdsLen;

	/*
	 * The interface for this is actually aweful
	 * nvDpyIdsLen will contain how many bytes are inside of nvDpyIds
	 * and the first element in nvDpyIds will tells us how many *elements* are in the array
	 * so on a two display system nvDpyIdsLen will be 12 bytes
	 * and nvDpyIds will contain 3 elements in the format of [length_of_this_array, first_dpy_id, second_dpy_id]
	*/
	XNVCTRLQueryBinaryData(dpy, screen, 0, NV_CTRL_BINARY_DATA_DISPLAYS_ENABLED_ON_XSCREEN,
							(unsigned char**)&nvDpyIds, &nvDpyIdsLen);
	dpys.reserve(nvDpyIds[0]);

	for(int i = 0; i < nvDpyIds[0]; i++){
		char *name;
		XNVCTRLQueryTargetStringAttribute(dpy, NV_CTRL_TARGET_TYPE_DISPLAY, nvDpyIds[i+1], 0,
											NV_CTRL_STRING_DISPLAY_NAME_RANDR, &name);
		dpys.emplace_back(nvidiaDisplay{name, nvDpyIds[i+1]});
		free(name);
	}

	free(nvDpyIds);

	return dpys;
}
