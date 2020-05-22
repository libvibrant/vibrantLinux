#include "autostart.h"

#include <QDir>

static QDir path(QDir::homePath() + "/.config/autostart");
static const QString filename = "vibrantLinux.desktop";
static const QString internalFilename(":/assets/autostart.desktop");

bool autostart::isEnabled() {
	// If file or autostart path doesn't exist
	if (!path.exists() || !path.exists(filename))
		return false;
	return true;
}

void autostart::enable() {
	if (!path.exists())
		QDir::home().mkpath(path.absolutePath());
	QFile::copy(internalFilename, path.absoluteFilePath(filename));
}

void autostart::disable() {
	QFile::remove(path.absoluteFilePath(filename));
}
