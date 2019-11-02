#include "utils.h"

QString pathToName(const QString &path){
	return path.split('/').last();
}

programInfo* getItemInfo(QListWidgetItem *item){
	return item->data(Qt::UserRole).value<programInfo*>();
}

QString& getItemPath(QListWidgetItem *item){
	return item->data(Qt::UserRole).value<programInfo*>()->path;
}

QString getItemName(QListWidgetItem *item){
	return pathToName(getItemPath(item));
}

int getItemDpyVibrance(QListWidgetItem *item, const QString &dpy){
	return item->data(Qt::UserRole).value<programInfo*>()->vibranceVals[dpy];
}

const QHash<QString, int>& getItemVibranceMap(QListWidgetItem *item){
	return item->data(Qt::UserRole).value<programInfo*>()->vibranceVals;
}
