#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow){
	ui->setupUi(this);

	displays = displayTab::getDisplays();
	for(auto &dpy: displays){
		ui->displays->addTab((QWidget*)dpy, dpy.getName());
	}

	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	if(!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		//create a default json file
		QDir dir = QDir::homePath();
		dir.mkdir(".config/vibrantLinux/");
		return;
	}
	settings = QJsonDocument::fromJson(settingsFile.readAll()).object();
	settingsFile.close();

	std::vector<displayTab> tmpDisplays;
	QJsonArray displayArr = settings["displays"].toArray();
	tmpDisplays.resize(size_t(displayArr.size()));
	for(auto dpyArr: displayArr){
		tmpDisplays.emplace_back();
		QJsonObject dpy = dpyArr.toObject();
		tmpDisplays.back().setName(dpy["name"].toString());
		tmpDisplays.back().setVibrance(dpy["vibrance"].toInt());
	}

	//if the monitor setup is the same then copy old settings, else reset everything
	if(displays == tmpDisplays){
		displays = tmpDisplays;
	}

	for(auto progArr: settings["programs"].toArray()){
		QJsonObject program = progArr.toObject();
		QString path = program["path"].toString();
		QMap<QString, int> vibrance;
		for(auto vibArr: program["vibrance"].toArray()){
			QJsonObject dpyVibrance = vibArr.toObject();
			vibrance.insert(dpyVibrance["name"].toString(), dpyVibrance["vibrance"].toInt());
		}
		addEntry(path, vibrance);
	}

	updateVibrance();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateVibrance()));
	timer->start(1000);

	systray.setIcon(QIcon(":/assets/icon.png"));
	connect(&systray, &QSystemTrayIcon::activated, this, &mainWindow::iconActivated);
	systray.show();
}

mainWindow::~mainWindow(){
	QString json = "{\"displays\":[";
	if(displays.size()){
		//displays.size()-1 so that the last one is written without the ",\n"
		for(size_t i = 0; i < displays.size()-1; i++){
			json += displays[i].toJson()+",";
		}
		json += displays.back().toJson();
	}
	json += "],";

	json += "\"programs\":[";
	int count = ui->programs->count();
	for(int i = 0; i < count-1; i++){
		json += itemToJson(ui->programs->item(i)) + ',';
	}
	//in case of no programs
	if(count > 0){
		json += itemToJson(ui->programs->item(count-1));
	}
	json += "]}\n";


	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	//conversion auto formats json
	settingsFile.open(QIODevice::WriteOnly);
	settingsFile.write(json.toUtf8());
	//destruct vector before displays
	displays.resize(0);
	delete timer;
	delete ui;
}

void mainWindow::addEntry(QString path){
	ui->programs->addItem(programName(path));
	QMap<QString, int> vibrance;
	for(auto &dpy: displays){
		vibrance.insert(dpy.getName(), dpy.getVibrance());
	}
	QListWidgetItem *item = ui->programs->item(ui->programs->count()-1);
	setItemPath(item, path);
	setItemMap(item, vibrance);
}

void mainWindow::addEntry(QString path, QMap<QString, int> vibrance){
	ui->programs->addItem(programName(path));
	QListWidgetItem *item = ui->programs->item(ui->programs->count()-1);
	setItemPath(item, path);
	setItemMap(item, vibrance);
}

void mainWindow::removeEntry(QListWidgetItem *item){
	ui->programs->takeItem(ui->programs->row(item));
}

void mainWindow::updateVibrance(){
	monitor.update();
	QListWidgetItem *program = monitor.getVibrance(ui->programs);
	if(program == nullptr){
		for(displayTab &dpy: displays){
			dpy.applyVibrance(displayTab::defaultVibrance);
		}
		return;
	}
	for(displayTab &dpy: displays){
		dpy.applyVibrance(itemVibrance(program, dpy.getName()));
	}
}

void mainWindow::on_addProgram_clicked(){
	QString program = QFileDialog::getOpenFileName(this, tr("Select a program"), QDir::homePath(),
		"Executable (*)", nullptr);
	if(program.isNull()){
		return;
	}

	addEntry(program);
}

void mainWindow::on_delProgram_clicked(){
	for(auto target: ui->programs->selectedItems()){
		for(int i = 0; i < ui->programs->count(); i++){
			if(ui->programs->item(i) == target){
				ui->programs->takeItem(i);
				break;
			}
		}
	}
}

void mainWindow::on_programs_doubleClicked(const QModelIndex &index){
	QListWidgetItem *item = ui->programs->item(index.row());
	entryEditor editor(item);
	editor.exec();
}

void mainWindow::on_actionSend_to_tray_triggered(){
	this->hide();
}

void mainWindow::on_donate_clicked(){
	QDesktopServices::openUrl(QUrl("https://paypal.me/vibrantlinux"));
}

void mainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
	if(reason == QSystemTrayIcon::ActivationReason::Trigger){
		if(!this->isVisible()){
			this->show();
		}
	}
}
