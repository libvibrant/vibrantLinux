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

#include "utils.h"
#include "entryeditor.h"
#include "procmonitor.h"
#include "displaytab.h"

namespace Ui {
class mainWindow;
}

class mainWindow : public QMainWindow{
	Q_OBJECT

public:
	explicit mainWindow(QWidget *parent = nullptr);
	~mainWindow();

private:
	void addEntry(QString path);
	void addEntry(QString path, QMap<QString, int> vibrance);
	void removeEntry(QListWidgetItem *item);

private slots:
	void updateVibrance();

	void on_addProgram_clicked();
	void on_delProgram_clicked();
	void on_programs_doubleClicked(const QModelIndex &index);
	void on_actionSend_to_tray_triggered();
	void on_donate_clicked();

	void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	Ui::mainWindow *ui;
	QJsonObject settings;
	procMonitor monitor;
	QSystemTrayIcon systray;
	QTimer *timer = nullptr;

	std::vector<displayTab> displays;
};

#endif // MAINWINDOW_H
