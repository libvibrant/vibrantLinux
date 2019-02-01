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

#include <filesystem>

#include "device.h"
#include "programentry.h"
#include "entryeditor.h"
#include "procmonitor.h"
#include "display.h"

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit mainWindow(QWidget *parent = nullptr);
	~mainWindow();

private:
	void applyVibrance(int vibrance);
	void addEntry(QString label, int vibrance = 0);
	void deleteEntry(QListWidgetItem *item);
	QString programName(QString path);

private slots:
	void updateVibrance();

	void on_vibranceSldr_valueChanged(int value);
	void on_vibranceVal_valueChanged(int value);
	void on_applyVibrance_clicked();
	void on_addProgram_clicked();
	void on_delProgram_clicked();
	void on_programs_doubleClicked(const QModelIndex &index);
	void on_actionSend_to_tray_triggered();
	void on_actionExit_triggered();
	void on_donate_clicked();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	Ui::mainWindow *ui;
	std::vector<programEntry> entries;
	QJsonObject settings;
	procMonitor monitor;
	QSystemTrayIcon systray;
	QTimer *timer = nullptr;
	int lastVibrance = 0;
	int defaultVibrance;

	std::vector<display> displays;
	QWidget *tabs;
};

#endif // MAINWINDOW_H
