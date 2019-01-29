#include "programentry.h"

#include <iostream>

programEntry::programEntry(QListWidgetItem *item, int vibrance){
	this->vibrance = vibrance;
	if(this->vibrance > 1023){
		this->vibrance = 1023;
	}
	else if(this->vibrance < -1024){
		this->vibrance = -1024;
	}

	this->path = item->text();
	this->item = item;
}

programEntry::programEntry(programEntry &&other) noexcept{
	item = other.item;
	path = other.path;
	vibrance = other.vibrance;

	other.item = nullptr;
}

programEntry::~programEntry(){
}

QString programEntry::toJson(){
	return QString("\n\t\t{\n\t\t\t\"vibrance\":")+QString::number(vibrance)+
			QString(",\n\t\t\t\"path\":\"")+path+"\"\n\t\t}";
}
