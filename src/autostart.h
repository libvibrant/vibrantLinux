#ifndef AUTOSTART_H
#define AUTOSTART_H

namespace Autostart {
bool isEnabled();
// returns false if we failed to write autostart file to disk
bool enable();
// returns false if we can't remove the autostart file for some reason
bool disable();
}; // namespace Autostart

#endif // AUTOSTART_H
