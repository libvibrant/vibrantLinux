#include "vibrantcontroller.h"

size_t vibrantController::instance_count = 0;
Display* vibrantController::dpy = nullptr;

vibrantController::vibrantController(RROutput output): output(output) {
	if(instance_count == 0){
		dpy = XOpenDisplay(nullptr);
		if(dpy == nullptr){
			throw std::runtime_error("failed to open connection to X server");
		}
	}
}

vibrantController::~vibrantController(){
	instance_count--;
	if(instance_count == 0){
		XCloseDisplay(dpy);
	}
}

int vibrantController::getSaturation(){
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

void vibrantController::setSaturation(int saturation){
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
