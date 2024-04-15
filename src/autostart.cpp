#include "autostart.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

const QString filename = "io.github.libvibrant.vibrantLinux.desktop";

auto autostartFilePath() -> QString {
  QDir configDir =
      QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
  QDir autostartDir = configDir.filePath("autostart");
  return autostartDir.filePath(filename);
}

auto Autostart::isEnabled() -> bool {
  return QFileInfo::exists(autostartFilePath());
}

auto Autostart::enable() -> bool {
  auto autostartFileInfo = QFileInfo(autostartFilePath());

  auto autostartDir = autostartFileInfo.dir();

  if (!autostartDir.exists()) {
    autostartDir.mkpath(".");
  };

  // If there is a desktop file on the system, link that to autostart
  {
    auto desktopFilePath =
        QStandardPaths::locate(QStandardPaths::ApplicationsLocation, filename,
                               QStandardPaths::LocateFile);

    if (!desktopFilePath.isEmpty()) {
      auto desktopFile = QFile(desktopFilePath);
      desktopFile.link(autostartFileInfo.absoluteFilePath());
      return true;
    }
  }

  // Create our own if there is no desktop file
  {
    auto internalDesktopFile = QFile(QString(":/%1.in").arg(filename));

    internalDesktopFile.open(QIODeviceBase::ReadOnly);
    QString desktopFileTemplate = internalDesktopFile.readAll();
    internalDesktopFile.close();

    auto desktopFileContents = desktopFileTemplate.replace(
        "Exec=vibrantLinux",
        QString("Exec=%1").arg(QCoreApplication::applicationFilePath()));

    auto autostartFile = QFile(autostartFileInfo.absoluteFilePath());
    autostartFile.open(QIODeviceBase::WriteOnly);
    if (autostartFile.write(desktopFileContents.toUtf8()) !=
        desktopFileContents.length()) {
      autostartFile.close();
      autostartFile.remove();
      qWarning() << "Couldn't write desktop file to" << autostartFileInfo;
      return false;
    };
    autostartFile.close();
    return true;
  }

  return false;
}

auto Autostart::disable() -> bool { return QFile::remove(autostartFilePath()); }
