#ifndef VIBRANTCONTROLLER_H
#define VIBRANTCONTROLLER_H

extern "C" {
    #include <vibrant/vibrant.h>
}

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

class vibrantController: public displayController{
public:
    vibrantController(RROutput output);
    ~vibrantController();

	int getSaturation();
    void setSaturation(int saturation);

private:
	//instances of this class will share the same Display * to send commands to the extension
	static size_t instance_count;
	static Display* dpy;
    RROutput output;
};

#endif // VIBRANTCONTROLLER_H
