#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QString>
#include <QHash>
#include <QMetaType>

struct programInfo{
	programInfo(const QString &path):
		path(path){}

	programInfo(const QString &path, const QHash<QString, int> &vibranceVals):
		path(path), vibranceVals(vibranceVals){}

	QString path;
	QHash<QString, int> vibranceVals;
};

Q_DECLARE_METATYPE(programInfo*)

#endif // PROGRAMINFO_H
