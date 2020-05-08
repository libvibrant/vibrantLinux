#include "displaymanager.h"

displayManager::displayManager(bool checkWindowInFocus): scanner(checkWindowInFocus){
	Display* dpy = XOpenDisplay(nullptr);
	if(dpy == nullptr){
		throw std::runtime_error("Failed to open connection to default X server");
	}

	int nvScreen;
	std::vector<nvidiaDisplay> nvDpys;

	//check if the extension exists on the current X server, and if we can find the nvidia screen
	if(nvidiaController::existsOnSystem(dpy) && (nvScreen = nvidiaController::getNvScreen(dpy)) != -1){
		nvDpys = nvidiaController::getDisplays(dpy, nvScreen);
	}

	Window root = DefaultRootWindow(dpy);
	XRRScreenResources* resources = XRRGetScreenResources(dpy, root);

	displays.reserve(resources->noutput);

	//get the list of displays
	for(int i = 0; i < resources->noutput; i++){
		RROutput output = resources->outputs[i];
		XRROutputInfo* info = XRRGetOutputInfo(dpy, resources, output);

		if(info->connection != RR_Disconnected){
			auto isNvDpy = [info](nvidiaDisplay &dpy) -> bool{
				return dpy.name == info->name;
			};
			auto isCTM = [dpy, output](){
				Atom prop_atom;

				// Find the X Atom associated with the property name
				prop_atom = XInternAtom(dpy, "CTM", 1);
				if (!prop_atom){
					return False;
				}

				// Make sure the property exists
				if (!XRRQueryOutputProperty(dpy, output, prop_atom)){
					return False;
				}

				return True;
			};

			std::vector<nvidiaDisplay>::iterator nvDpy;

			//only add the display to the list of display names if we have a controller for it
			if(isCTM()){
                auto ctm = new (std::nothrow) ctmController(output);
                if(ctm == nullptr){
                    throw std::runtime_error("failed to allocate memory for a display controller");
                }
                controllers.insert(info->name, ctm);
                displays.append(info->name);
			}
			else if((nvDpy = std::find_if(nvDpys.begin(), nvDpys.end(), isNvDpy)) != nvDpys.end()){
				auto nv = new (std::nothrow) nvidiaController(*nvDpy);
				if(nv == nullptr){
					throw std::runtime_error("failed to allocate memory for a display controller");
				}

                controllers.insert(nvDpy->name, nv);
                displays.append(info->name);
            }
		}

		XRRFreeOutputInfo(info);
	}

	XRRFreeScreenResources(resources);
	XCloseDisplay(dpy);
}

displayManager::~displayManager(){
}

QStringList displayManager::getDisplayNames(){
	return displays;
}

int displayManager::getDisplaySaturation(const QString& name){
	return controllers[name]->getSaturation();
}

void displayManager::updateSaturation(QListWidget* watchlist){
	auto info = scanner.getSaturation(watchlist);
	if(info != nullptr){
		for(auto it = info->saturationVals.begin(); it != info->saturationVals.end(); it++){
			controllers[it.key()]->setSaturation(it.value());
		}
	}
	else{
		for(auto controller: controllers){
			controller->setSaturation(controller->defaultSaturation);
		}
	}
}

void displayManager::checkWindowFocus(bool use){
	scanner.setCheckWindowFocus(use);
}

bool displayManager::isCheckingWindowFocus(){
	return scanner.isCheckingWindowFocus();
}

void displayManager::setDefaultDisplaySaturation(const QString& name, int value){
	controllers[name]->defaultSaturation = value;
}
