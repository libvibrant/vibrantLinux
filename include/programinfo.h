#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QString>
#include <QHash>
#include <QMetaType>

class programInfo{
public:
	programInfo(const QString &path, const QHash<QString, int> &saturationVals):
		path(path), saturationVals(saturationVals){}

	static QString exeNameFromPath(const QString &path){
		return path.split('/').last();
	}

	QString path;
	QHash<QString, int> saturationVals;
};

Q_DECLARE_METATYPE(programInfo*)

#endif // PROGRAMINFO_H
