#ifndef DISPLAYTAB_H
#define DISPLAYTAB_H

#include <stdexcept>
#include <vector>

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QProcess>
#include <QString>
#include <QStringList>

#include "utils.h"

class displayTab : public QWidget{
	Q_OBJECT
public:
	displayTab(QString name, QWidget *parent = nullptr, bool getVibrance = true);
	displayTab(const displayTab &other);
	displayTab(displayTab &&other) noexcept;
	~displayTab();

	displayTab& operator=(const displayTab &other){
		if(this == &other){
			return *this;
		}

		slider->setValue(other.slider->value());
		spinBox->setValue(other.spinBox->value());
		name = other.name;
		currentVibrance = other.currentVibrance;

		return *this;
	}

	displayTab& operator=(displayTab &&other){
		if(this == &other){
			return *this;
		}

		delete label;
		delete slider;
		delete spinBox;
		delete layout;

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

		return *this;
	}

	bool operator==(const displayTab &other) const{
		return name == other.name;
	}

	void applyVibrance(int vibrance);

	static QStringList getDisplayNames();

	int getDefaultVibrance();
	void setDefaultVibrance(int value);

	int getCurrentVibrance();

	const QString& getName();
	void setName(QString name);

private:
	void makeTab();

	QGridLayout *layout = nullptr;
	QLabel *label = nullptr;
	QSlider *slider = nullptr;
	QSpinBox *spinBox = nullptr;
	QString name;
	//set it outside of range so the first call to applyVibrance always works
	//this is the currently applied vibrance
	int currentVibrance;
};

#endif // DISPLAYTAB_H
