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

#include "entryeditor.h"
#include "displaytab.h"
#include "displaymanager.h"
#include "autostart.h"
#include <algorithm>

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
	void setupFromConfig();
	//helper functions for setupFromConfig
	bool monitorSetupChanged(const QJsonArray &configDisplays);
	QJsonObject generateConfig();
	void writeConfig();
	void loadAutostartState();

	void addEntry(programInfo info);
	void removeEntry(QListWidgetItem *item);

	Ui::mainWindow *ui;

	displayManager manager;
	QStringList displayNames;
	QTimer timer;

	QMenu systrayMenu;
private slots:
	void updateSaturation();
	void defaultSaturationChanged(const QString &name, int value);

	/*we have to keep the function in case VIBRANT_LINUX_NO_XCB is defined
	otherwise moc_mainwindow will contain an undefined refernece to it*/
	void on_vibranceFocusToggle_clicked(bool checked);
	void on_addProgram_clicked();
	void on_delProgram_clicked();
	void on_programs_doubleClicked(const QModelIndex &index);

	void on_actionShowHideWindow_triggered();
	void on_actionExit_triggered();
	void on_actionAbout_triggered();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void toggleRunOnStartup();
};

#endif // MAINWINDOW_H
