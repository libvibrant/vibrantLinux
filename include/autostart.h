#ifndef AUTOSTART_H
#define AUTOSTART_H

#include <QDir>

class autostart
{
public:
	static bool isEnabled();
	static void enable();
	static void disable();
};

#endif // AUTOSTART_H
