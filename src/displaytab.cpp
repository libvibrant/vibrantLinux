#include "displaytab.h"

displayTab::displayTab(QString name, QWidget *parent, bool getVibrance) : QWidget(parent), name(name){
	makeTab();

	if(getVibrance){
		currentVibrance = getNvidiaSettingsVibrance(name);
	}
}

displayTab::displayTab(const displayTab &other) : QWidget (other.parentWidget()){
	makeTab();
	slider->setValue(other.slider->value());
	spinBox->setValue(other.spinBox->value());
	name = other.name;
	currentVibrance = other.currentVibrance;
}

displayTab::displayTab(displayTab &&other) noexcept{
	label = other.label;
	slider = other.slider;
	spinBox = other.spinBox;
	layout = other.layout;
	name = std::move(other.name);
	currentVibrance = other.currentVibrance;

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

void displayTab::makeTab(){
	label = new (std::nothrow) QLabel("Vibrance", this);
	if(!label){
		throw std::runtime_error("failed to allocate memory for label");
	}

	slider = new (std::nothrow) QSlider(Qt::Orientation::Horizontal, this);
	if(!slider){
		delete label;
		throw std::runtime_error("failed to allocate memory for slider");
	}
	slider->setRange(-1024, 1023);

	spinBox = new (std::nothrow) QSpinBox(this);
	if(!spinBox){
		delete label;
		delete slider;
		throw std::runtime_error("failed to allocate memory for spinbox");
	}
	spinBox->setRange(-1024, 1023);

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

	connect(spinBox, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
	connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
}

void displayTab::applyVibrance(int vibrance){
	QString nvidiaCall = "nvidia-settings -a ["+name+"]";
	nvidiaCall += "/DigitalVibrance="+QString::number(vibrance);
	system(nvidiaCall.toUtf8());
	currentVibrance = vibrance;
}

QStringList displayTab::getDisplayNames(){
	QStringList names;

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
	* [0] HOSTNAME:0[dpy:NUMBER] (CONNECTION_NAME) (connected, enabled)
	*/
	QStringList res = QString(grep.readAll()).split("\n");

	for(int i = 0; i < res.size() - 1; i++){
		/*im sorry i know this looks horrendous
		splits the string on the first ( which will come right before CONNECTION_NAME
		then splits it again at ) which results in the first element of the string list being CONNECTION_NAME*/
		names.append(res[i].split("(")[1].split(")")[0]);
	}

	return names;
}

int displayTab::getNvidiaSettingsVibrance(const QString &name){
	QProcess nvidia;
	QProcess grep;
	nvidia.setStandardOutputProcess(&grep);
	nvidia.start("nvidia-settings -q [" + name + "]/DigitalVibrance");
	nvidia.waitForFinished();
	grep.start("grep dpy");
	grep.waitForFinished();

	/*
	* The output should be in this format
	* Attribute 'DigitalVibrance' (HOSTNAME:0[dpy:NUMBER]): 0.
	*/

	QString res = grep.readAllStandardOutput().split(' ').last();
	//remove the period at the end
	res.resize(res.size()-1);
	return res.toInt();
}

int displayTab::getDefaultVibrance(){
	return spinBox->value();
}

void displayTab::setDefaultVibrance(int value){
	spinBox->setValue(value);
	slider->setValue(value);
}

int displayTab::getCurrentVibrance(){
	return currentVibrance;
}

const QString& displayTab::getName(){
	return name;
}

void displayTab::setName(QString name){
	this->name = name;
}
