#include "autostart.h"

#include <QDir>

QDir path(QDir::homePath() + "/.config/autostart");
const QString filename = "vibrantLinux.desktop";
const QByteArray autostart_file_content =
    "[Desktop Entry]\n"
    "Version=1.0\n"
    "Name=vibrantLinux\n"
    "GenericName=Digital vibrance controller\n"
    "Comment=Program-specific digital vibrance controller\n"
    "Exec=/usr/bin/vibrantLinux --hidden\n"
    "Icon=vibrantLinux\n"
    "Terminal=false\n"
    "Type=Application\n"
    "Categories=Utility;\n";

static const QString internalFilename(":/autostart.desktop");

bool autostart::isEnabled() {
  // If file or autostart path doesn't exist
  if (!path.exists() || !path.exists(filename)) {
    return false;
  }
  return true;
}

bool autostart::enable() {
  if (!path.exists()) {
    QDir::home().mkpath(path.absolutePath());
  }

  QFile autostart(path.absoluteFilePath(filename));
  autostart.open(QFile::WriteOnly);
  if (!autostart.isOpen()) {
    return false;
  }

  if (autostart.write(autostart_file_content) !=
      autostart_file_content.size()) {
    autostart.remove();
    return false;
  }

  return true;
}

bool autostart::disable() {
  return QFile::remove(path.absoluteFilePath(filename));
}
