#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QStringList>
#include <QHash>
#include <QListWidget>

#include "nvidiacontroller.h"
#include "vibrantcontroller.h"
#include "procscanner.h"

class displayManager{
public:
	displayManager(bool checkWindowInFocus);
	~displayManager();

	QStringList getDisplayNames();
	int getDisplaySaturation(const QString& name);

	void updateSaturation(QListWidget* watchlist);

	void checkWindowFocus(bool use);
	bool isCheckingWindowFocus();

	//creates or updates an entry in the watchlist
	void setDefaultDisplaySaturation(const QString& name, int value);

private:
	procScanner scanner;

	QStringList displays;
	QHash<QString, displayController*> controllers;
};

#endif // DISPLAYMANAGER_H
