#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>

#ifndef VIBRANT_LINUX_NO_XCB
#include <xcb/xcb_ewmh.h>
#endif

#include <vector>

#include "utils.h"

class procScanner{
public:
	procScanner();
	~procScanner();

	//if this returns nullptr it means no programs in the watch list are running
	QListWidgetItem* getVibrance(QListWidget *&watchList);

	#ifndef VIBRANT_LINUX_NO_XCB
	bool establishXCon();

	void setUseX(bool use);
	bool isUsingX();
	bool isConnectedToX();
	#endif

private:
	std::vector<QString> processes;

	#ifndef VIBRANT_LINUX_NO_XCB
	bool useX = false;

	bool connectedToX = false;
	xcb_ewmh_connection_t xcon;
	#endif
};

#endif // PROCMONITOR_H
