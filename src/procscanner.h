#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QRegularExpression>

#include "programinfo.h"

#include <xcb/xcb_ewmh.h>

class procScanner{
public:
	procScanner(bool checkWindowFocus);
	~procScanner();

	//if this returns nullptr it means no programs in the watch list are running
	const programInfo* getSaturation(QListWidget* watchlist);

	bool establishXCon();

	void setCheckWindowFocus(bool use);
	bool isCheckingWindowFocus();

private:
	std::vector<QString> processes;

	bool checkWindowFocus = false;
	bool connectedToX = false;
	xcb_ewmh_connection_t xcon;
};

#endif // PROCMONITOR_H
