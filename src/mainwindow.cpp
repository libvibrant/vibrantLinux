#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <algorithm>

const int CURRENT_CONFIG_VER = 2;

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::mainWindow) {
  ui->setupUi(this);

  m_configDir =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
  QDir configDir(m_configDir);
  m_configPath = configDir.filePath("vibrantLinux.internal");

  qDebug() << "Resolved config path:" << m_configPath;

  QIcon icon =
      QIcon::fromTheme("io.github.libvibrant.vibrantLinux",
                       QIcon(":/io.github.libvibrant.vibrantLinux.png"));
  setWindowIcon(icon);
  systray.setIcon(icon);

  connect(&systray, &QSystemTrayIcon::activated, this,
          &mainWindow::iconActivated);

  systrayMenu.addAction(ui->actionShowHideWindow);
  systrayMenu.addAction(ui->actionExit);
  systray.setContextMenu(&systrayMenu);

  displayNames = manager.getDisplayNames();

  qDebug() << "Detected displays:" << displayNames;

  setupFromConfig();
  ui->actionRunOnStartup->setChecked(autostart::isEnabled());

  for (int i = 0; i < ui->displays->count(); i++) {
    auto tab = ui->displays->widget(i);
    if (tab == ui->noDisplaysTab)
      continue;

    auto dpyTab = dynamic_cast<DisplayTab *>(ui->displays->widget(i));
    connect(dpyTab, &DisplayTab::onSaturationChange, this,
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

  DisplayTab *tmp;
  while ((tmp = dynamic_cast<DisplayTab *>(ui->displays->widget(0))) !=
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
  std::vector<DisplayTab *> tabs;
  tabs.reserve(displayNames.size());
  auto alloc_err = "Failed to allocate memory for display tabs";
  for (auto &name : displayNames) {
    auto dpyTab =
        new DisplayTab(name, manager.getDisplaySaturation(name), this);
    if (!dpyTab) {
      QMessageBox::warning(this, "Failed to create display tab", alloc_err);
      throw std::runtime_error(alloc_err);
    }
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
  } else {
    qDebug() << "Config doesn't exist";
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
    for (const auto &displayEntry : settings.value("displays").toArray()) {
      const auto &displaySettings = displayEntry.toObject();
      const auto displayName = displaySettings["name"].toString();
      const auto displaySaturation = displaySettings["vibrance"].toInt();

      qDebug() << "Found display" << displayName << "in config";

      const auto tab =
          std::find_if(tabs.begin(), tabs.end(), [&displayName](auto tab) {
            return tab->name == displayName;
          });
      if (tab == tabs.end()) {
        qDebug() << "Display" << displayName
                 << "not found on display server. Configuration value:"
                 << displaySettings;
        continue;
      }

      (*tab)->setSaturation(displaySaturation);
      manager.setDefaultDisplaySaturation(displayName, displaySaturation);
      connect(*tab, &DisplayTab::onSaturationChange,
              [this](const QString &name, int value) {
                manager.setDefaultDisplaySaturation(name, value);
              });
    }
  }
  {
    for (const auto &programEntry : settings.value("programs").toArray()) {
      const auto &programSettings = programEntry.toObject();
      auto programMatch = programSettings["matchString"].toString();
      auto programType =
          programInfo::stringToEntryType(programSettings["type"].toString());

      qDebug() << "Found program" << programSettings["matchString"]
               << "in config";

      QHash<QString, int> vibranceValues;
      for (auto displayEntry : programSettings["vibrance"].toArray()) {
        const auto &displaySettings = displayEntry.toObject();
        const auto displayName = displaySettings["name"].toString();
        const auto displaySaturation = displaySettings["vibrance"].toInt();

        qDebug() << "Found display" << displayName << "for program"
                 << programMatch << "in config";

        const auto tab =
            std::find_if(tabs.begin(), tabs.end(), [&displayName](auto tab) {
              return tab->name == displayName;
            });
        if (tab == tabs.end()) {
          qDebug() << "Display" << displayName << "for program" << programMatch
                   << "not found on display server. Configuration value:"
                   << displaySettings;
          continue;
        }
        vibranceValues.insert(displayName, displaySaturation);
      }
      addEntry(programInfo(programType, programMatch, vibranceValues));
    }
  }

  if (tabs.size() > 0) {
    ui->displays->setTabBarAutoHide(false);
    ui->displays->removeTab(0);
  }
  for (auto tab : tabs) {
    ui->displays->addTab(tab, tab->name);
  }
}

void mainWindow::writeConfig() {
  if (!m_loaded) {
    // writeConfig can be called while we are loading the config.
    // Not everything might be loaded by now, so let's ignore this
    qWarning() << "Refusing to save config";
    return;
  }
  qDebug() << "Saving config";

  QJsonObject obj;
  obj.insert("configVersion", CURRENT_CONFIG_VER);

  // use to temporarily store monitor list while we add to it
  QJsonArray tmpArr;
  for (int i = 0; i < ui->displays->count(); i++) {
    auto tab = ui->displays->widget(i);
    if (tab == ui->noDisplaysTab)
      continue;
    DisplayTab *dpy = dynamic_cast<DisplayTab *>(tab);
    QJsonObject tmpObj;
    tmpObj.insert("name", dpy->name);
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
  writeConfig();
  systray.hide();
  close();
}

void mainWindow::on_actionAbout_triggered() {
  QMessageBox::about(
      this, "About",
      QString("vibrantLinux is a program to automatically set the color "
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
