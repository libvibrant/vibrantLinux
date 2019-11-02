#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDesktopServices>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QMenu>

#ifndef VIBRANT_LINUX_NO_XCB
#include <xcb/xcb_ewmh.h>
#endif

#include "entryeditor.h"
#include "procscanner.h"
#include "displaytab.h"
#include "programinfo.h"
#include "utils.h"

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit mainWindow(QWidget *parent = nullptr);
	~mainWindow();

	QSystemTrayIcon systray;

private:
	void parseConfig();
	//helper functions for generateFromConfig
	bool hasMonitorSetupChanged(const QJsonArray &configDisplays, const QStringList &currentDisplays);
	QJsonObject generateConfig(const QStringList &displayNames);
	void generateTabs(const QStringList &displayNames);
	void freeAllocatedTabs();

	void addEntry(const QString &path);
	void addEntry(const QString &path, const QHash<QString, int> &vibrance);
	void removeEntry(QListWidgetItem *item);

	#ifndef VIBRANT_LINUX_NO_XCB
	bool establishXConnection();
	#endif

	Ui::mainWindow *ui;
	procScanner monitor;
	QTimer timer;

	#ifndef VIBRANT_LINUX_NO_XCB
	bool connectedToX = false;
	xcb_ewmh_connection_t xcon;
	#endif

	QMenu systrayMenu;

private slots:
	void updateVibrance();

	/*we have to keep the function in case VIBRANT_LINUX_NO_XCB is defined
	otherwise moc_mainwindow will contain an undefined refernece to it*/
	void on_vibranceFocusToggle_clicked(bool checked);
	void on_addProgram_clicked();
	void on_delProgram_clicked();
	void on_programs_doubleClicked(const QModelIndex &index);

	void on_actionShowHideWindow_triggered();
	void on_actionExit_triggered();
	void on_actionAbout_triggered();

	void on_donate_clicked();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // MAINWINDOW_H
