#include "displaytab.h"

displayTab::displayTab(QString name, QWidget *parent) : QWidget(parent), name(name){
	makeTab();
}

displayTab::displayTab(const displayTab &other) : QWidget (other.parentWidget()){
	makeTab();
	slider->setValue(other.slider->value());
	spinBox->setValue(other.spinBox->value());
	name = other.name;
	saturation = other.saturation;
}

displayTab::displayTab(displayTab &&other) noexcept{
	label = other.label;
	slider = other.slider;
	spinBox = other.spinBox;
	layout = other.layout;
	name = std::move(other.name);
	saturation = other.saturation;

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
}

int displayTab::getSaturation(){
	return spinBox->value();
}

void displayTab::setSaturation(int saturation){
	spinBox->setValue(saturation);
}

const QString& displayTab::getName(){
	return name;
}

void displayTab::setName(QString name){
	this->name = name;
}

void displayTab::saturationChanged(int value){
	spinBox->setValue(value);
	slider->setValue(value);

	emit onSaturationChange(name, value);
}

void displayTab::makeTab(){
	label = new (std::nothrow) QLabel(QString("Vibrance for %1").arg(name), this);
	if(!label){
		throw std::runtime_error("failed to allocate memory for label");
	}

	slider = new (std::nothrow) QSlider(Qt::Orientation::Horizontal, this);
	if(!slider){
		delete label;
		throw std::runtime_error("failed to allocate memory for slider");
	}
	slider->setRange(0, 400);

	spinBox = new (std::nothrow) QSpinBox(this);
	if(!spinBox){
		delete label;
		delete slider;
		throw std::runtime_error("failed to allocate memory for spinbox");
	}
	spinBox->setRange(0, 400);

	layout = new (std::nothrow) QGridLayout();
	if(!layout){
		delete label;
		delete slider;
		delete spinBox;
		throw std::runtime_error("failed to allocate memory for layout");
	}

	layout->addWidget(label, 0, 0);
	layout->addWidget(slider, 1, 0);
	layout->addWidget(spinBox, 1, 1);
	setLayout(layout);

	connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), this, &displayTab::saturationChanged);
	connect(slider, &QSlider::valueChanged, this, &displayTab::saturationChanged);
}
