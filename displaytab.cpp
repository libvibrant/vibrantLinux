#include "displaytab.h"
#include <iostream>

QString displayTypeToStr(displayType type){
	static char const *translate[] = {"nvidia", "amd", "intel"};
	return translate[int(type)];
}

displayType strToDisplayType(QString str){
	if(str == "nvidia"){
		return displayType::nvidia;
	} else if(str == "amd"){
		return displayType::amd;
	} else if(str == "intel"){
		return displayType::intel;
	}
	return displayType::unkown;
}

displayTab::displayTab(QWidget *parent) : QWidget(parent){
	delete label;
	delete slider;
	delete spinBox;
	delete layout;
	delete tab;
	makeTab(parent);
}

displayTab::displayTab(const displayTab &other) : QWidget (other.parentWidget()){
	makeTab(other.parentWidget());
	slider->setValue(other.slider->value());
	spinBox->setValue(other.spinBox->value());
	name = other.name;
	type = other.type;
	lastVibrance = other.lastVibrance;
}

displayTab::displayTab(displayTab &&other) noexcept{
	tab = other.tab;
	label = other.label;
	slider = other.slider;
	spinBox = other.spinBox;
	layout = other.layout;
	type = other.type;
	name = std::move(other.name);
	lastVibrance = other.lastVibrance;

	other.tab = nullptr;
	other.label = nullptr;
	other.slider = nullptr;
	other.spinBox = nullptr;
	other.layout = nullptr;
}

displayTab::~displayTab(){
	delete label;
	delete slider;
	delete spinBox;
	delete layout;
	delete tab;
}

void displayTab::applyVibrance(int vibrance){
	if(vibrance == defaultVibrance){
		vibrance = spinBox->value();
	}
	if(vibrance == lastVibrance){
		return;
	}
	if(type == displayType::nvidia){
		QString syscall = "nvidia-settings -a ["+name+"]";
		syscall += "/DigitalVibrance="+QString::number(vibrance);
		system(syscall.toUtf8());
	}
	lastVibrance = vibrance;
}

std::vector<displayTab> displayTab::getDisplays(QWidget *parent){
	std::vector<displayTab> tabs;
	if(systemHasNvidia()){
		QProcess nvidia;
		QProcess grep;
		nvidia.setStandardOutputProcess(&grep);
		nvidia.start("nvidia-settings -q dpys");
		grep.start("grep connected");
		grep.setProcessChannelMode(QProcess::ForwardedChannels);
		nvidia.waitForFinished();
		grep.waitForFinished();

		/*
		* The output should be in this format
		* [0] HOSTNAME:1[dpy:0] (DVI-D-0) (connected, enabled)
		*
		* note that the extra new line above was left on purpose
		* nvidia puts an extra new line at the end of the command
		*/
		QStringList res = QString(grep.readAll()).split("\n");

		for(int i = 0; i < res.size() - 1; i++){
			//im sorry i know this looks horrendous
			QString displayName = res[i].split("(")[1].split(")")[0];

			tabs.emplace_back(parent);
			tabs.back().setName(displayName);
			tabs.back().setType(displayType::nvidia);
		}
	}
	return tabs;
}

std::vector<QString> displayTab::getDisplayNames(){
	std::vector<QString> names;
	if(systemHasNvidia()){
		QProcess nvidia;
		QProcess grep;
		nvidia.setStandardOutputProcess(&grep);
		nvidia.start("nvidia-settings -q dpys");
		grep.start("grep connected");
		grep.setProcessChannelMode(QProcess::ForwardedChannels);
		nvidia.waitForFinished();
		grep.waitForFinished();
		QStringList res = QString(grep.readAll()).split("\n");

		for(int i = 0; i < res.size() - 1; i++){
			//im sorry i know this looks horrendous
			names.emplace_back(res[i].split("(")[1].split(")")[0]);
		}
	}
	return names;
}

QString displayTab::toJson(){
	QString res = "{";
	res += "\"name\": " + ('\"' + name + '\"') + ",";
	res += "\"type\": \"" + displayTypeToStr(type) + "\",";
	res += "\"vibrance\": " + QString::number(spinBox->value());
	res += "}";
	return res;
}

int displayTab::getVibrance(){
	return spinBox->value();
}

void displayTab::setVibrance(int value){
	spinBox->setValue(value);
	slider->setValue(value);
}

QString displayTab::getName(){
	return name;
}

void displayTab::setName(QString name){
	this->name = name;
}

displayType displayTab::getType(){
	return type;
}

void displayTab::setType(displayType type){
	this->type = type;
}

void displayTab::makeTab(QWidget *parent){
	tab = new QWidget(parent);
	if(!tab){
		throw std::runtime_error("failed to allocate memory for tab");
	}

	label = new QLabel("Vibrance", tab);
	if(!label){
		throw std::runtime_error("failed to allocate memory for label");
	}

	slider = new QSlider(Qt::Orientation::Horizontal, tab);
	if(!slider){
		throw std::runtime_error("failed to allocate memory for slider");
	}
	slider->setRange(-1024, 1023);

	spinBox = new QSpinBox(tab);
	if(!spinBox){
		throw std::runtime_error("failed to allocate memory for spinbox");
	}
	spinBox->setRange(-1024, 1023);

	layout = new QGridLayout();
	layout->addWidget(label, 0, 0);
	layout->addWidget(slider, 1, 0);
	layout->addWidget(spinBox, 1, 1);
	tab->setLayout(layout);

	connect(spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
	connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
}
