#ifndef DISPLAYTAB_H
#define DISPLAYTAB_H

#include <QString>
#include <QWidget>

#include "ui_displaytab.h"

class DisplayTab : public QWidget {
  Q_OBJECT
public:
  DisplayTab(QString name, int initialSaturation, QWidget *parent = nullptr);
  ~DisplayTab() = default;

  bool operator==(const DisplayTab &other) const { return name == other.name; }

  int getSaturation();
  void setSaturation(int saturation);

  QString name;

private slots:
  void saturationChanged(int value);

signals:
  void onSaturationChange(const QString &name, int value);

private:
  Ui::DisplayTab *ui;
};

#endif // DISPLAYTAB_H
