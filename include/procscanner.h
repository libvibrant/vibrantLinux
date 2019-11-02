#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QListWidget>

#include <vector>

#include "utils.h"

class procScanner{
public:
	procScanner();
	~procScanner();

	//if this returns nullptr it means no programs in the watch list are running
	QListWidgetItem* getVibrance(QListWidget *&watchList);

private:
	void updateProcessesVector();
	std::vector<QString> processes;
};

#endif // PROCMONITOR_H
