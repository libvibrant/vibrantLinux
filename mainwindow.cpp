#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow){
	ui->setupUi(this);

	displays = display::getDisplays();
	tabs = new QWidget[displays.size()];
	for(size_t i = 0; i < displays.size(); i++){
		ui->displays->addTab(tabs+i, QString::fromStdString(displays[i].displayName));
	}

	systray.setIcon(QIcon("assets/icon.png"));
	connect(&systray, &QSystemTrayIcon::activated, this, &mainWindow::iconActivated);

	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	if(!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		//create a default json file
		QDir dir = QDir::homePath();
		dir.mkdir(".config/vibrantLinux/");
		settingsFile.open(QIODevice::WriteOnly | QIODevice::Text);
		QByteArray raw = QJsonDocument::fromJson("{\"vibrance\":0,\"programs\":[]}").toJson();
		settingsFile.write(raw, raw.length());
		return;
	}
	settings = QJsonDocument::fromJson(settingsFile.readAll()).object();
	settingsFile.close();

	ui->vibranceSldr->setValue(settings["vibrance"].toInt());
	ui->vibranceVal->setValue(ui->vibranceSldr->value());
	defaultVibrance = ui->vibranceSldr->value();
	for(auto val: settings["programs"].toArray()){
		QJsonObject program = val.toObject();
		addEntry(program["path"].toString(), program["vibrance"].toInt());
	}

	applyVibrance(defaultVibrance);

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(updateVibrance()));
	timer->start(1000);
}

mainWindow::~mainWindow(){
	QString json = "{\n\t\"vibrance\": ";
	json += QString::number(ui->vibranceSldr->value())+",";
	json += "\n\t\"programs\":\n\t[";
	//in case of no programs
	if(entries.size()){
		//entries.size()-1 so that the last one is written without the ",\n"
		for(size_t i = 0; i < entries.size()-1; i++){
			json += "\n\t\t"+entries[i].toJson()+",\n";
		}
		json += entries[entries.size()-1].toJson();
	}
	json += "\n\t]\n}\n";


	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	settingsFile.open(QIODevice::WriteOnly);
	settingsFile.write(json.toUtf8(), json.length());
	delete[] tabs;
	delete timer;
	delete ui;
}

void mainWindow::applyVibrance(int vibrance){
	for(display &dpy: displays){
		dpy.applyVibrance(vibrance);
	}
}

void mainWindow::addEntry(QString label, int vibrance){
	ui->programs->addItem(programName(label));
	entries.emplace_back(ui->programs->item(ui->programs->count()-1), label, vibrance);
}

void mainWindow::deleteEntry(QListWidgetItem *item){
	for(auto it = entries.begin(); it != entries.end(); it++){
		if(*it == item){
			entries.erase(it);
			ui->programs->removeItemWidget(item);
			delete item;
			break;
		}
	}
}

QString mainWindow::programName(QString path){
	QStringList pieces = path.split("/");
	return pieces.value(pieces.length()-1);
}

void mainWindow::updateVibrance(){
	monitor.update();
	int vibrance = monitor.getVibrance(entries);
	if(vibrance == procMonitor::defaultVibrance){
		vibrance = defaultVibrance;
	}
	if(vibrance != lastVibrance){
		applyVibrance(vibrance);
	}
}

void mainWindow::on_vibranceSldr_valueChanged(int value){
	ui->vibranceVal->setValue(value);
}

void mainWindow::on_vibranceVal_valueChanged(int value){
	ui->vibranceSldr->setValue(value);
}

void mainWindow::on_applyVibrance_clicked(){
	defaultVibrance = ui->vibranceSldr->value();
	applyVibrance(defaultVibrance);
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
		deleteEntry(target);
	}
}

void mainWindow::on_programs_doubleClicked(const QModelIndex &index){
	QListWidgetItem *item = ui->programs->item(index.row());
	for(size_t i = 0; i < entries.size(); i++){
		if(entries[i] == item){
			entryEditor editor(&entries[i]);
			editor.exec();
			item->setText(programName(entries[i].path));
		}
	}
}

void mainWindow::on_actionSend_to_tray_triggered(){
	systray.show();
	this->hide();
}

void mainWindow::on_actionExit_triggered(){
}

void mainWindow::on_donate_clicked(){
	QDesktopServices::openUrl(QUrl("https://paypal.me/vibrantlinux"));
}

void mainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
	if(reason == QSystemTrayIcon::ActivationReason::Trigger){
		systray.hide();
		this->show();
	}
}
