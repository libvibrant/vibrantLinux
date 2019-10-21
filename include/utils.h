#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QListWidgetItem>

#include "programinfo.h"

QString pathToName(QString path);

programInfo* getItemInfo(QListWidgetItem *item);

QString getItemPath(QListWidgetItem *item);

QString getItemName(QListWidgetItem *item);

int* getItemDpyVibrance(QListWidgetItem *item, QString dpy);

QMap<QString, int>* getItemVibranceMap(QListWidgetItem *item);

#endif // UTILS_H
