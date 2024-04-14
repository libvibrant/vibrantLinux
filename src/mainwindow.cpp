#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>

const int CURRENT_CONFIG_VER = 2;

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::mainWindow) {
  ui->setupUi(this);

  m_configDir =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir configDir(m_configDir);
  m_configPath = configDir.filePath("vibrantLinux.internal");

  QIcon icon =
      QIcon::fromTheme("io.github.libvibrant.vibrantLinux",
                       QIcon(":/assets/io.github.libvibrant.vibrantLinux.png"));
  setWindowIcon(icon);
  systray.setIcon(icon);

  connect(&systray, &QSystemTrayIcon::activated, this,
          &mainWindow::iconActivated);

  systrayMenu.addAction(ui->actionShowHideWindow);
  systrayMenu.addAction(ui->actionExit);
  systray.setContextMenu(&systrayMenu);

  displayNames = manager.getDisplayNames();
  setupFromConfig();
  ui->actionRunOnStartup->setChecked(autostart::isEnabled());

  for (int i = 0; i < ui->displays->count(); i++) {
    auto dpyTab = dynamic_cast<displayTab *>(ui->displays->widget(i));
    connect(dpyTab, &displayTab::onSaturationChange, this,
            &mainWindow::defaultSaturationChanged);
  }

  systray.show();

  connect(&timer, &QTimer::timeout, this, &mainWindow::updateSaturation);
  timer.start(1000);

  m_loaded = true;
}

mainWindow::~mainWindow() {
  QListWidgetItem *item;
  while ((item = ui->programs->item(0)) != nullptr) {
    removeEntry(item);
  }

  displayTab *tmp;
  while ((tmp = dynamic_cast<displayTab *>(ui->displays->widget(0))) !=
         nullptr) {
    ui->displays->removeTab(0);
    delete tmp;
  }

  delete ui;
}

void mainWindow::setupFromConfig() {
  QJsonObject settings;

  // keep in a vector for now since we'll probably be searching through them,
  // add them to ui->displays at the end
  std::vector<displayTab *> tabs;
  tabs.reserve(displayNames.size());
  auto alloc_err = "Failed to allocate memory for display tabs";
  for (auto &name : displayNames) {
    displayTab *dpyTab = new (std::nothrow) displayTab(name);
    if (dpyTab == nullptr) {
      displayTab *tmp;
      while ((tmp = dynamic_cast<displayTab *>(ui->displays->widget(0))) !=
             nullptr) {
        ui->displays->removeTab(0);
        delete tmp;
      }

      QMessageBox::warning(this, "Failed to create display tab", alloc_err);
      throw std::runtime_error(alloc_err);
    }

    dpyTab->setSaturation(manager.getDisplaySaturation(name));
    tabs.push_back(dpyTab);
  }

  QFile settingsFile(m_configPath);

  int configVersion = CURRENT_CONFIG_VER;
  // load config if it exists
  if (settingsFile.exists()) {
    settingsFile.open(QFile::ReadOnly);
    settings = QJsonDocument::fromJson(settingsFile.readAll()).object();
    settingsFile.close();

    configVersion = settings.value("configVersion").toInt(-1);
  }

  // this is unused for now, but we'll use it later for whenever the config
  // format changes
  if (configVersion < 0) {
    // this is useless for now, but will be used later on when config versions
    // are updated
    auto err = "Unrecognized config version, please delete "
               "~/.config/vibrantLinux/vibrantLinux.internal and try again";
    QMessageBox::critical(this, "Unrecognized config version", err);
    throw std::runtime_error(err);
  }
  Q_UNUSED(configVersion);

  {
    bool useWindowFocus = settings.value("UseWindowFocus").toBool(true);
    ui->vibranceFocusToggle->setChecked(useWindowFocus);
  }

  {
    auto configDisplaysArr = settings.value("displays").toArray(QJsonArray());

    if (monitorSetupChanged(configDisplaysArr)) {
      // displayNames that have carried over from the setup change
      std::vector<displayTab *> sameDisplays;
      for (auto tab : tabs) {
        for (auto configDpyRef : configDisplaysArr) {
          auto configDpy = configDpyRef.toObject();
          if (tab->getName() == configDpy["name"].toString()) {
            sameDisplays.push_back(tab);

            int saturation = configDpy["vibrance"].toInt();
            tab->setSaturation(saturation);
            manager.setDefaultDisplaySaturation(tab->getName(), saturation);
          }
        }
      }

      for (auto programRef : settings["programs"].toArray()) {
        QJsonObject program = programRef.toObject();
        QHash<QString, int> vibranceVals;
        auto type = programInfo::stringToEntryType(program["type"].toString());
        ;

        for (auto vibranceRef : program["vibrance"].toArray()) {
          QJsonObject vibrance = vibranceRef.toObject();
          auto name = vibrance["name"].toString();
          // check if name is in our sameDisplays
          auto findTabFn = [name](displayTab *tab) {
            return name == tab->getName();
          };
          auto tab =
              std::find_if(sameDisplays.begin(), sameDisplays.end(), findTabFn);

          if (tab != sameDisplays.end()) {
            // old config stored values in the range of [0, 400]
            int val = vibrance["vibrance"].toInt();
            vibranceVals.insert(name, val);
          } else {
            /*
             * tab is not in sameDisplays, its either a new display or a display
             * that was removed check if we have the display, if we do then set
             * its saturation the displays current saturation if not just move
             * one
             */

            // if true then this is a new display, add it to the programs info,
            // if not we'll just toss it away
            if ((tab = std::find_if(tabs.begin(), tabs.end(), findTabFn)) !=
                tabs.end()) {
              vibranceVals.insert(name, (*tab)->getSaturation());
            }
          }
        }

        addEntry(
            programInfo(type, program["matchString"].toString(), vibranceVals));
      }
    } else {
      for (auto tab : tabs) {
        for (auto configDpyRef : configDisplaysArr) {
          auto configDpy = configDpyRef.toObject();
          auto name = configDpy["name"].toString();
          if (tab->getName() == name) {
            int saturation = configDpy["vibrance"].toInt();
            tab->setSaturation(saturation);
            manager.setDefaultDisplaySaturation(name, saturation);
          }
        }
      }

      auto configProgramsArr = settings.value("programs").toArray(QJsonArray());

      for (auto programRef : configProgramsArr) {
        QJsonObject program = programRef.toObject();
        QHash<QString, int> vibranceVals;
        auto type = programInfo::stringToEntryType(program["type"].toString());
        ;

        for (auto vibranceRef : program["vibrance"].toArray()) {
          QJsonObject vibrance = vibranceRef.toObject();
          // old config stored values in the range of [-100, 100]
          int val = vibrance["vibrance"].toInt();

          vibranceVals.insert(vibrance["name"].toString(), val);
        }

        addEntry(
            programInfo(type, program["matchString"].toString(), vibranceVals));
      }
    }
  }

  for (auto tab : tabs) {
    ui->displays->addTab(tab, tab->getName());
  }
}

bool mainWindow::monitorSetupChanged(const QJsonArray &configDisplays) {
  if (configDisplays.size() == displayNames.size()) {
    for (auto dpy : configDisplays) {
      if (!displayNames.contains(dpy.toObject()["name"].toString())) {
        return true;
      }
    }

    return false;
  }

  return true;
}

void mainWindow::writeConfig() {
  if (!m_loaded) {
    // writeConfig can be called while we are loading the config.
    // Not everything might be loaded by now, so let's ignore this
    return;
  }

  QJsonObject obj;
  obj.insert("configVersion", CURRENT_CONFIG_VER);

  // use to temporarily store monitor list while we add to it
  QJsonArray tmpArr;
  for (int i = 0; i < ui->displays->count(); i++) {
    displayTab *dpy = dynamic_cast<displayTab *>(ui->displays->widget(i));
    QJsonObject tmpObj;
    tmpObj.insert("name", dpy->getName());
    tmpObj.insert("vibrance", dpy->getSaturation());

    tmpArr.append(tmpObj);
  }
  // add monitor list to config object
  obj.insert("displays", tmpArr);

  // clear array
  tmpArr = QJsonArray();

  // convert programs to json array
  for (int i = 0; i < ui->programs->count(); i++) {
    QListWidgetItem *item = ui->programs->item(i);
    programInfo *info = item->data(Qt::UserRole).value<programInfo *>();

    QJsonObject program;
    QJsonArray programVibrance;

    program.insert("type", programInfo::entryTypeToString(info->type));
    program.insert("matchString", QString(info->matchString));

    for (auto i = info->saturationVals.begin(); i != info->saturationVals.end();
         i++) {
      QJsonObject vibranceObj;
      vibranceObj.insert("name", i.key());
      vibranceObj.insert("vibrance", i.value());

      programVibrance.append(vibranceObj);
    }
    program.insert("vibrance", programVibrance);

    tmpArr.append(program);
  }
  obj.insert("programs", tmpArr);

  obj.insert("UseWindowFocus", ui->vibranceFocusToggle->isChecked());

  QDir settingsDir(m_configDir);
  settingsDir.mkpath("."); // ensure config path exists

  QFile settingsFile(m_configPath);
  // conversion auto formats json
  settingsFile.open(QIODevice::WriteOnly);
  settingsFile.write(QJsonDocument(obj).toJson());
  settingsFile.close();
}

void mainWindow::on_actionRunOnStartup_triggered(bool checked) {
  if (checked) {
    if (!autostart::enable()) {
      ui->actionRunOnStartup->setChecked(false);
      QMessageBox::warning(this, "Failed to create file",
                           "Failed to create autostart file");
    }
  } else {
    if (!autostart::disable()) {
      ui->actionRunOnStartup->setChecked(true);
      QMessageBox::warning(this, "Failed to remove file",
                           "Failed to remove autostart file");
    }
  }
}

void mainWindow::addEntry(programInfo info) {
  QListWidgetItem *item;
  if (info.type == programInfo::entryType::MatchPath) {
    item = new (std::nothrow)
        QListWidgetItem(programInfo::exeNameFromPath(info.matchString));
  } else {
    item = new (std::nothrow) QListWidgetItem(info.matchString);
  }
  auto itemInfo = new (std::nothrow) programInfo{info};

  auto alloc_err = "Failed to allocate memory for new item entry";
  if (item == nullptr || itemInfo == nullptr) {
    QMessageBox::warning(this, "Not enough memory", alloc_err);
    throw std::runtime_error(alloc_err);
  }

  item->setData(Qt::UserRole, QVariant::fromValue(itemInfo));
  ui->programs->addItem(item);
}

void mainWindow::removeEntry(QListWidgetItem *item) {
  ui->programs->takeItem(ui->programs->row(item));
  auto info = item->data(Qt::UserRole).value<programInfo *>();

  delete info;
  delete item;
}

void mainWindow::updateSaturation() { manager.updateSaturation(ui->programs); }

void mainWindow::defaultSaturationChanged(const QString &name, int value) {
  manager.setDefaultDisplaySaturation(name, value);
}

void mainWindow::on_vibranceFocusToggle_toggled(bool checked) {
  manager.checkWindowFocus(checked);
  // user tried to turn on ewmh features but programScanner failed to set it up
  if (checked) {
    if (!manager.isCheckingWindowFocus()) {
      ui->vibranceFocusToggle->setDisabled(true);
      ui->vibranceFocusToggle->setChecked(false);
      ui->vibranceFocusToggle->setToolTip(
          "Failed to enable focus checking, are you running an ewmh X server?");
      return;
    }
  }

  writeConfig();
}

void mainWindow::on_addProgram_clicked() {
  programInfo info(programInfo::entryType::MatchPath, "",
                   QHash<QString, int>());

  entryEditor editor(info, displayNames, this);
  if (editor.exec() == QDialog::Accepted) {
    addEntry(info);
    writeConfig();
  }
}

void mainWindow::on_delProgram_clicked() {
  auto items = ui->programs->selectedItems();
  if (items.size()) {
    for (auto &item : items) {
      removeEntry(item);
    }
    writeConfig();
  }
}

void mainWindow::on_programs_doubleClicked(const QModelIndex &index) {
  QListWidgetItem *item = ui->programs->item(index.row());
  auto info = item->data(Qt::UserRole).value<programInfo *>();

  entryEditor editor(*info, displayNames, this);
  if (editor.exec() == QDialog::Accepted) {
    if (info->type == programInfo::entryType::MatchPath) {
      item->setText(programInfo::exeNameFromPath(info->matchString));
    } else {
      item->setText(info->matchString);
    }

    writeConfig();
  }
}

void mainWindow::on_actionShowHideWindow_triggered() {
  setVisible(!isVisible());
}

void mainWindow::on_actionExit_triggered() {
  systray.hide();
  close();
}

void mainWindow::on_actionAbout_triggered() {
  QMessageBox::about(
      this, "About",
      QString("vigrantLinux is a program to automatically set the color "
              "saturation "
              "of specific monitors depending on what program is current "
              "running.\n\n"
              "This program currently works with NVIDIA, and any GPU that "
              "implements "
              "the Color Transformation Matrix (CTM) property.\n\nVersion: %1")
          .arg(VIBRANT_LINUX_VERSION));
}

void mainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
    if (!isVisible()) {
      show();
    }
  }
}
