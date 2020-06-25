#ifndef AUTOSTART_H
#define AUTOSTART_H

namespace autostart{
	bool isEnabled();
	//returns false if we failed to write autostart file to disk
	bool enable();
	//returns false if we can't remove the autostart file for some reason
	bool disable();
};

#endif // AUTOSTART_H
