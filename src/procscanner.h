#ifndef PROCMONITOR_H
#define PROCMONITOR_H

#include <QDir>
#include <QFileInfo>
#include <QListWidget>
#include <QRegularExpression>
#include <QString>

#include <xcb/xcb_ewmh.h>

#include "programinfo.h"

class ProcScanner {
public:
  ProcScanner(bool checkWindowFocus);
  ~ProcScanner();

  // if this returns nullptr it means no programs in the watch list are running
  const ProgramInfo *getSaturation(QListWidget *watchlist);

  bool establishXCon();

  void setCheckWindowFocus(bool use);
  bool isCheckingWindowFocus();

private:
  std::vector<QString> processes;

  bool checkWindowFocus = false;
  bool connectedToX = false;
  xcb_ewmh_connection_t xcon;
};

#endif // PROCMONITOR_H
