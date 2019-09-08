#ifndef DEVICE_H
#define DEVICE_H

#include <unistd.h>
#include <string>
#include <QString>
#include <QStringList>
#include <QListWidgetItem>

bool systemHasNvidia();

QString programName(QString path);

int itemVibrance(const QListWidgetItem *item, QString key);
void setItemVibrance(QListWidgetItem *item, QString key, int val);

QMap<QString, int> itemMap(const QListWidgetItem *item);
void setItemMap(QListWidgetItem *item, QMap<QString, int> map);

QString itemPath(const QListWidgetItem *item);
/*
 *this also modifies the item's text. For example if the text is "ls" and the path is "/bin/ls"
 *and this function is called with the path value "/bin/mkdir" the items text will be changed to "mkdir"
*/
void setItemPath(QListWidgetItem *item, QString path);

QString itemToJson(const QListWidgetItem *item);

#endif // DEVICE_H
