#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QString>
#include <QMap>
#include <QMetaType>

struct programInfo{
	programInfo(const QString &path):
		path(path){}

	programInfo(const QString &path, const QMap<QString, int> &vibranceVals):
		path(path), vibranceVals(vibranceVals){}

	QString path;
	QMap<QString, int> vibranceVals;
};

Q_DECLARE_METATYPE(programInfo*)

#endif // PROGRAMINFO_H
