#include "ctmcontroller.h"

size_t ctmController::instance_count = 0;
Display* ctmController::dpy = nullptr;

ctmController::ctmController(RROutput output): output(output) {
	if(instance_count == 0){
		dpy = XOpenDisplay(nullptr);
		if(dpy == nullptr){
			throw std::runtime_error("failed to open connection to X server");
		}
	}
}

ctmController::~ctmController(){
	instance_count--;
	if(instance_count == 0){
		XCloseDisplay(dpy);
	}
}

int ctmController::getSaturation(){
    int x_status;
    double saturation = get_saturation(dpy, output, &x_status);
    int roundedSaturation = qRound(saturation * 100) - 100;
    // normalize saturation to be between -100 and 100, 0 being neutral
    if (roundedSaturation <= 0) {
        return roundedSaturation;
    } else {
        return roundedSaturation / 3;
    }
}

void ctmController::setSaturation(int saturation){
    int x_status;
    // convert normalized value to a value vibrant can work with
    double actualSaturation;
    if (saturation <= 0) {
        actualSaturation = saturation;
    } else {
        actualSaturation = saturation * 3.0;
    }
    actualSaturation += 100.0;
    actualSaturation /= 100.0;
    set_saturation(dpy, output, actualSaturation, &x_status);
}
