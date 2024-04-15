#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaType>

class ProgramInfo {
public:
  enum EntryType { MatchPath, MatchTitle, SubMatchTitle, RegexMatchTitle };

  // can't really add a fromJson ctor type thing because it needs to take into
  // account changing monitor configs
  ProgramInfo(EntryType type, const QString &matchString,
              const QHash<QString, int> &saturationVals)
      : type(type), matchString(matchString.toUtf8()),
        saturationVals(saturationVals) {}

  ProgramInfo(const ProgramInfo &other) {
    type = other.type;
    matchString = other.matchString;
    saturationVals = other.saturationVals;
  }

  ProgramInfo(ProgramInfo &&other) {
    type = other.type;
    matchString = std::move(other.matchString);
    saturationVals = std::move(other.saturationVals);
  }

  QJsonObject toJson() {
    QJsonObject ret;

    ret.insert("type", type);
    ret.insert("matchString", QString(matchString));
    QJsonArray programs;
    for (auto it = saturationVals.begin(); it != saturationVals.end(); it++) {
      QJsonObject saturationInfo;
      saturationInfo.insert("name", it.key());
      saturationInfo.insert("vibrance", it.value());
    }

    return ret;
  }

  static QString exeNameFromPath(const QString &path) {
    return path.split('/').last();
  }

  static QString entryTypeToString(EntryType type) {
    switch (type) {
    case MatchPath:
      return "MatchPath";
    case MatchTitle:
      return "MatchTitle";
    case SubMatchTitle:
      return "SubMatchTitle";
    case RegexMatchTitle:
      return "RegexMatchTitle";
    default:
      throw std::runtime_error("how did you even manage to do this? this was "
                               "just here to stop gcc from complaining");
    }
  }

  static EntryType stringToEntryType(const QString &string) {
    if (string == "MatchPath") {
      return EntryType::MatchPath;
    } else if (string == "MatchTitle") {
      return EntryType::MatchTitle;
    } else if (string == "SubMatchTitle") {
      return EntryType::SubMatchTitle;
    } else if (string == "RegexMatchTitle") {
      return EntryType::RegexMatchTitle;
    }

    throw std::runtime_error("invalid string");
  }

  EntryType type;
  // cant use QString because it stores stuff as utf-16, we need it to be utf-8
  QByteArray matchString;
  QHash<QString, int> saturationVals;
};

Q_DECLARE_METATYPE(ProgramInfo *)

#endif // PROGRAMINFO_H
