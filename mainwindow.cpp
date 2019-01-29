#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow){
	ui->setupUi(this);
	findDevicesNvidia(nvidiaDevs);

	QFile settingsFile(".vibrantLinux");
	if(!settingsFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		//create a default json file
		settingsFile.open(QIODevice::WriteOnly);
		QByteArray raw = QJsonDocument::fromJson("{\"vibrance\":0,\"programs\":[]}").toJson();
		settingsFile.write(raw, raw.length());
		return;
	}
	settings = QJsonDocument::fromJson(settingsFile.readAll()).object();
	settingsFile.close();

	ui->vibranceSldr->setValue(settings["vibrance"].toInt());
	ui->vibranceVal->setValue(ui->vibranceSldr->value());
	for(auto val: settings["programs"].toArray()){
		QJsonObject program = val.toObject();
		addEntry(program["path"].toString(), program["vibrance"].toInt());
	}
	applyVibrance(ui->vibranceSldr->value());
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


	QFile settingsFile(".vibrantLinux");
	settingsFile.open(QIODevice::WriteOnly);
	settingsFile.write(json.toUtf8(), json.length());
	delete ui;
}

void mainWindow::applyVibrance(int vibrance){
	if(nvidiaDevs.size()){
		std::string command = std::string("nvidia-settings -a \"DigitalVibrance=")+std::to_string(vibrance)+"\"";
		system(command.c_str());
	}
}

void mainWindow::addEntry(QString label, int vibrance){
	ui->programs->addItem(programName(label));
	entries.emplace_back(ui->programs->item(ui->programs->count()-1), vibrance);
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

void mainWindow::on_vibranceSldr_valueChanged(int value){
	ui->vibranceVal->setValue(value);
}

void mainWindow::on_vibranceVal_valueChanged(int value){
	ui->vibranceSldr->setValue(value);
}

void mainWindow::on_applyVibrance_clicked(){
	applyVibrance(ui->vibranceSldr->value());
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

void mainWindow::on_donate_clicked(){
	QDesktopServices::openUrl(QUrl("https://paypal.me/vibrantlinux"));
}
