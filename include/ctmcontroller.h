#ifndef VIBRANTCONTROLLER_H
#define VIBRANTCONTROLLER_H

#include <vibrant/vibrant.h>

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

class ctmController: public displayController{
public:
	ctmController(RROutput output);
	~ctmController();

	int getSaturation();
	void setSaturation(int saturation);

private:
	//instances of this class will share the same Display * to send commands to the extension
	static size_t instance_count;
	static Display* dpy;
	RROutput output;
};

#endif // VIBRANTCONTROLLER_H
