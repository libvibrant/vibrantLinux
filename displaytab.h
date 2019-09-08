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
	explicit displayTab(QWidget *parent = nullptr);
	explicit displayTab(const displayTab &other);
	explicit displayTab(displayTab &&other) noexcept;
	~displayTab();

	operator QWidget*(){
		return tab;
	}

	displayTab& operator=(const displayTab &other){
		if(this == &other){
			return *this;
		}

		makeTab(other.parentWidget());
		slider->setValue(other.slider->value());
		spinBox->setValue(other.spinBox->value());
		name = other.name;
		lastVibrance = other.lastVibrance;

		return *this;
	}

	displayTab& operator=(displayTab &&other){
		if(this == &other){
			return *this;
		}

		tab = other.tab;
		label = other.label;
		slider = other.slider;
		spinBox = other.spinBox;
		layout = other.layout;
		name = std::move(other.name);
		lastVibrance = other.lastVibrance;

		other.tab = nullptr;
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

	static std::vector<displayTab> getDisplays(QWidget *parent = nullptr);
	static std::vector<QString> getDisplayNames();

	QString toJson();

	int getVibrance();
	void setVibrance(int value);

	QString getName();
	void setName(QString name);

private:
	void makeTab(QWidget *parent = nullptr);

public:
	static const int defaultVibrance = 1024;
private:
	QWidget *tab = nullptr;
	QGridLayout *layout = nullptr;
	QLabel *label = nullptr;
	QSlider *slider = nullptr;
	QSpinBox *spinBox = nullptr;
	QString name;
	int lastVibrance = defaultVibrance;
};

#endif // DISPLAYTAB_H
