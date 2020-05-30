#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QByteArray>
#include <QHash>
#include <QMetaType>
#include <QJsonObject>
#include <QJsonArray>

class programInfo{
public:
	enum entryType{
		MatchPath,
		MatchTitle,
		SubMatchTitle,
		RegexMatchTitle
	};

	//can't really add a fromJson ctor type thing because it needs to take into account changing monitor configs
	programInfo(entryType type, const QString &matchString, const QHash<QString, int> &saturationVals):
	type(type), matchString(matchString.toUtf8()), saturationVals(saturationVals){
	}

	programInfo(const programInfo& other){
		type = other.type;
		matchString = other.matchString;
		saturationVals = other.saturationVals;
	}

	programInfo(programInfo&& other){
		type = other.type;
		matchString = std::move(other.matchString);
		saturationVals = std::move(other.saturationVals);
	}

	QJsonObject toJson(){
		QJsonObject ret;

		ret.insert("type", type);
		ret.insert("matchString", QString(matchString));
		QJsonArray programs;
		for(auto it = saturationVals.begin(); it != saturationVals.end(); it++){
			QJsonObject saturationInfo;
			saturationInfo.insert("name", it.key());
			saturationInfo.insert("vibrance", it.value());
		}

		return ret;
	}

	static QString exeNameFromPath(const QString &path){
		return path.split('/').last();
	}

	static QString entryTypeToString(entryType type){
		switch(type){
			case MatchPath:
				return "MatchPath";
			case MatchTitle:
				return "MatchTitle";
			case SubMatchTitle:
				return "SubMatchTitle";
			case RegexMatchTitle:
				return "RegexMatchTitle";
			default:
				throw std::runtime_error("how did you even manage to do this? this was just here to stop gcc from complaining");
		}
	}

	static entryType stringToEntryType(const QString &string){
		if(string == "MatchPath"){
			return entryType::MatchPath;
		}
		else if(string == "MatchTitle"){
			return entryType::MatchTitle;
		}
		else if(string == "SubMatchTitle"){
			return entryType::SubMatchTitle;
		}
		else if(string == "RegexMatchTitle"){
			return entryType::RegexMatchTitle;
		}

		throw std::runtime_error("invalid string");
	}

	entryType type;
	//cant use QString because it stores stuff as utf-16, we need it to be utf-8
	QByteArray matchString;
	QHash<QString, int> saturationVals;
};

Q_DECLARE_METATYPE(programInfo*)

#endif // PROGRAMINFO_H
