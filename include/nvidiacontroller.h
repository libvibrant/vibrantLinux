#ifndef NVIDIACONTROLLER_H
#define NVIDIACONTROLLER_H

#include <X11/extensions/Xrandr.h>
#include <NVCtrl/NVCtrlLib.h>

#undef Bool
#undef CursorShape
#undef Expose
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef None
#undef Status
#undef Unsorted

#include "displaycontroller.h"

struct nvidiaDisplay{
	QString name;
	int id;
};

class nvidiaController: public displayController{
public:
	nvidiaController(nvidiaDisplay &display);
	~nvidiaController();

	int getSaturation();
	void setSaturation(int saturation);

	static bool existsOnSystem(Display *dpy);
	static int getNvScreen(Display *dpy);
	static std::vector<nvidiaDisplay> getDisplays(Display *dpy, int screen);

private:
	//instances of this class will share the same Display * to send commands to the extension
	static size_t instance_count;
	static Display* dpy;
	nvidiaDisplay display;
};

#endif // NVIDIACONTROLLER_H
