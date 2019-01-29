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

#include "device.h"
#include "programentry.h"
#include "entryeditor.h"

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
	void on_vibranceSldr_valueChanged(int value);
	void on_vibranceVal_valueChanged(int value);
	void on_applyVibrance_clicked();
	void on_addProgram_clicked();
	void on_delProgram_clicked();
	void on_programs_doubleClicked(const QModelIndex &index);
	void on_donate_clicked();

private:
	Ui::mainWindow *ui;
	std::vector<device> nvidiaDevs;
	std::vector<programEntry> entries;
	QJsonObject settings;
};

#endif // MAINWINDOW_H
