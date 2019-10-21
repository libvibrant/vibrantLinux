#include "utils.h"

QString pathToName(QString path){
	return path.split('/').last();
}

programInfo* getItemInfo(QListWidgetItem *item){
	return item->data(Qt::UserRole).value<programInfo*>();
}

QString getItemPath(QListWidgetItem *item){
	return item->data(Qt::UserRole).value<programInfo*>()->path;
}

QString getItemName(QListWidgetItem *item){
	return pathToName(getItemPath(item));
}

int* getItemDpyVibrance(QListWidgetItem *item, QString dpy){
	return &item->data(Qt::UserRole).value<programInfo*>()->vibranceVals[dpy];
}

QMap<QString, int>* getItemVibranceMap(QListWidgetItem *item){
	return &item->data(Qt::UserRole).value<programInfo*>()->vibranceVals;
}
