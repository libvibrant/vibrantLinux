#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>
#include <QFileDialog>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDesktopServices>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMap>
#include <QMessageBox>
#include <QMenu>

#include <xcb/xcb_ewmh.h>

#include "entryeditor.h"
#include "procmonitor.h"
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
	bool establishXConnection();

	void addEntry(const QString &path);
	void addEntry(const QString &path, const QMap<QString, int> &vibrance);
	void removeEntry(QListWidgetItem *item);

	Ui::mainWindow *ui;
	procMonitor monitor;
	QTimer *timer = nullptr;

	bool connectedToX = false;
	xcb_ewmh_connection_t xcon;

	QMenu *systrayMenu;

private slots:
	void updateVibrance();

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
