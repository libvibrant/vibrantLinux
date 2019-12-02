#include "mainwindow.h"
#include "ui_mainwindow.h"

mainWindow::mainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow){
	ui->setupUi(this);

	QIcon icon = QIcon::fromTheme("vibrantLinux", QIcon(":/assets/icon.png"));
	setWindowIcon(icon);
	systray.setIcon(icon);

	connect(&systray, &QSystemTrayIcon::activated, this, &mainWindow::iconActivated);

	systrayMenu.addAction(ui->actionShowHideWindow);
	systrayMenu.addAction(ui->actionExit);
	systray.setContextMenu(&systrayMenu);

	parseConfig();

	systray.show();

	#ifndef VIBRANT_LINUX_NO_XCB
	//try to establish an X connection, if we can't then scan the /proc/ folder every second
	if(!programScanner.isConnectedToX()){
		ui->vibranceFocusToggle->setCheckState(Qt::Unchecked);
		ui->vibranceFocusToggle->setEnabled(false);
	}
	#else
	ui->vibranceFocusToggle->setCheckState(Qt::Unchecked);
	ui->vibranceFocusToggle->setEnabled(false);
	#endif

	connect(&timer, &QTimer::timeout, this, &mainWindow::updateVibrance);
	timer.start(1000);
}

mainWindow::~mainWindow(){
	writeConfig();

	QListWidgetItem *item;
	while((item = ui->programs->item(0)) != nullptr){
		removeEntry(item);
	}

	//destruct vector before displays
	displayTab *dpy;
	while((dpy = dynamic_cast<displayTab*>(ui->displays->widget(0))) != nullptr){
		ui->displays->removeTab(0);
		delete dpy;
	}

	delete ui;
}

void mainWindow::parseConfig(){
	QJsonObject settings;
	QStringList displayNames = displayTab::getDisplayNames();

	//check if config file exists, and if it does read it. Otherwise generate one
	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	if(QFile::exists(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal")){
		settingsFile.open(QFile::ReadOnly);
		settings = QJsonDocument::fromJson(settingsFile.readAll()).object();
		QJsonArray displaysArray = settings["displays"].toArray();

		if(hasMonitorSetupChanged(displaysArray, displayNames)){
			generateTabs(displayNames);

			for(auto programRef: settings["programs"].toArray()){
				QJsonObject program = programRef.toObject();
				addEntry(program["path"].toString());
			}
		}
		else{
			for(auto dpyRef: displaysArray){
				QJsonObject dpy = dpyRef.toObject();
				displayTab *dpyTab = new (std::nothrow) displayTab(dpy["name"].toString());
				if(dpyTab == nullptr){
					freeAllocatedTabs();
					throw std::runtime_error("failed to allocate memory for display tabs");
				}

				int vibrance = dpyTab->getCurrentVibrance();
				dpyTab->setDefaultVibrance(vibrance);
				ui->displays->addTab(dpyTab, dpyTab->getName());
			}

			for(auto programRef: settings["programs"].toArray()){
				QJsonObject program = programRef.toObject();
				QHash<QString, int> vibranceVals;

				for(auto vibranceRef: program["vibrance"].toArray()){
					QJsonObject vibrance = vibranceRef.toObject();
					vibranceVals.insert(vibrance["name"].toString(), vibrance["vibrance"].toInt());
				}

				addEntry(program["path"].toString(), vibranceVals);
			}
		}
	}
	else{
		QDir dir = QDir::homePath();
		dir.mkpath(".config/vibrantLinux/");
		settingsFile.open(QFile::WriteOnly);

		settings = generateConfig(displayNames);
		settingsFile.write(QJsonDocument(settings).toJson());
		settingsFile.close();

		//if this throws let it propagate to the ctor
		generateTabs(displayNames);
	}
}

bool mainWindow::hasMonitorSetupChanged(const QJsonArray &configDisplays, const QStringList &currentDisplays){
	if(configDisplays.size() == currentDisplays.size()){
		for(auto dpy: configDisplays){
			if(!currentDisplays.contains(dpy.toObject()["name"].toString())){
				return true;
			}
		}

		return false;
	}

	return true;
}

QJsonObject mainWindow::generateConfig(const QStringList &displayNames){
	QJsonObject res;

	//store the displays as an array
	QJsonArray tmpArr;
	for(auto &name: displayNames){
		QJsonObject dpyObject;
		dpyObject.insert("name", name);
		dpyObject.insert("vibrance", displayTab::getNvidiaSettingsVibrance(name));

		tmpArr.append(dpyObject);
	}
	res.insert("displays", tmpArr);

	//clear the array
	tmpArr = QJsonArray();

	//create an empty programs array
	res.insert("programs", tmpArr);

	return res;
}

void mainWindow::generateTabs(const QStringList &displayNames){
	for(auto &name: displayNames){
		displayTab *dpyTab = new (std::nothrow) displayTab(name);
		if(dpyTab == nullptr){
			freeAllocatedTabs();
			throw std::runtime_error("failed to allocate memory for display tabs");
		}

		int vibrance = dpyTab->getCurrentVibrance();
		dpyTab->setDefaultVibrance(vibrance);
		ui->displays->addTab(dpyTab, name);
	}
}

void mainWindow::freeAllocatedTabs(){
	displayTab *tmp;
	while((tmp = dynamic_cast<displayTab*>(ui->displays->widget(0))) != nullptr){
		ui->displays->removeTab(0);
		delete tmp;
	}
}

void mainWindow::writeConfig(){
	QJsonObject obj;
	QJsonArray tmpArr;
	for(int i = 0; i < ui->displays->count(); i++){
		displayTab *dpy = dynamic_cast<displayTab*>(ui->displays->widget(i));
		QJsonObject tmpObj;
		tmpObj.insert("name", dpy->getName());
		tmpObj.insert("vibrance", dpy->getDefaultVibrance());

		tmpArr.append(tmpObj);
	}
	obj.insert("displays", tmpArr);

	//clear array
	tmpArr = QJsonArray();

	//convert programs to json array
	for(int i = 0; i < ui->programs->count(); i++){
		QListWidgetItem *item = ui->programs->item(i);
		programInfo *info = getItemInfo(item);

		QJsonObject program;
		QJsonArray programVibrance;
		program.insert("path", info->path);
		for(auto i = info->vibranceVals.begin(); i != info->vibranceVals.end(); i++){
			QJsonObject vibranceObj;
			vibranceObj.insert("name", i.key());
			vibranceObj.insert("vibrance", i.value());

			programVibrance.append(vibranceObj);
		}
		program.insert("vibrance", programVibrance);

		tmpArr.append(program);
	}
	obj.insert("programs", tmpArr);


	QFile settingsFile(QDir::homePath()+"/.config/vibrantLinux/vibrantLinux.internal");
	//conversion auto formats json
	settingsFile.open(QIODevice::WriteOnly);
	settingsFile.write(QJsonDocument(obj).toJson());
	settingsFile.close();
}

void mainWindow::addEntry(const QString &path){

	//create a new item
	QListWidgetItem *item = new (std::nothrow) QListWidgetItem(pathToName(path));
	if(item == nullptr){
		QMessageBox::warning(this, "Not enough memory", "Failed to allocate memory for new item entry");
		return;
	}

	//create userdata for the item
	programInfo *info = new (std::nothrow) programInfo(path);
	if(info == nullptr){
		delete item;
		QMessageBox::warning(this, "Not enough memory", "Failed to allocate memory for new item entry");
		return;
	}

	//assign a vibrance value to each display
	for(int i = 0; i < ui->displays->count(); i++){
		displayTab *dpy = dynamic_cast<displayTab*>(ui->displays->widget(0));
		info->vibranceVals.insert(dpy->getName(), dpy->getDefaultVibrance());
	}

	item->setData(Qt::UserRole, QVariant::fromValue(info));

	ui->programs->addItem(item);
}

void mainWindow::addEntry(const QString &path, const QHash<QString, int> &vibrance){
	QListWidgetItem *item = new (std::nothrow) QListWidgetItem(pathToName(path));
	if(item == nullptr){
		QMessageBox::warning(this, "Not enough memory", "Failed to allocate memory for new item entry");
		return;
	}

	programInfo *info = new (std::nothrow) programInfo(path, vibrance);
	if(info == nullptr){
		delete item;
		QMessageBox::warning(this, "Not enough memory", "Failed to allocate memory for new item entry");
		return;
	}

	item->setData(Qt::UserRole, QVariant::fromValue(info));

	ui->programs->addItem(item);
}

void mainWindow::removeEntry(QListWidgetItem *item){
	ui->programs->takeItem(ui->programs->row(item));

	delete getItemInfo(item);
	delete item;
}
void mainWindow::updateVibrance(){
	auto program = programScanner.getVibrance(ui->programs);
	if(program == nullptr){
		for(int i = 0; i < ui->displays->count(); i++){
			displayTab *dpy = dynamic_cast<displayTab*>(ui->displays->widget(i));

			int vibrance = dpy->getDefaultVibrance();
			if(dpy->getCurrentVibrance() != vibrance){
				dpy->applyVibrance(vibrance);
			}
		}
	}
	else{
		for(int i = 0; i < ui->displays->count(); i++){
			displayTab *dpy = dynamic_cast<displayTab*>(ui->displays->widget(i));

			int vibrance = getItemDpyVibrance(program, dpy->getName());
			if(dpy->getCurrentVibrance() != vibrance){
				dpy->applyVibrance(vibrance);
			}
		}
	}
}

void mainWindow::on_vibranceFocusToggle_clicked(bool checked){
	#ifndef VIBRANT_LINUX_NO_XCB
	programScanner.setUseX(checked);
	//user tried to turn on ewmh features but it programScanner failed to set it up
	if(checked){
		if(!programScanner.isUsingX()){
			ui->vibranceFocusToggle->setCheckState(Qt::Unchecked);
		}
	}
	#else
	Q_UNUSED(checked)
	#endif
}

void mainWindow::on_addProgram_clicked(){
	QString program = QFileDialog::getOpenFileName(this, tr("Select a program"), QDir::homePath(),
		"Executable (*)", nullptr);
	if(program.isNull()){
		return;
	}

	auto fileInfo = QFileInfo(program);
	if(fileInfo.isSymLink()){
		program = fileInfo.symLinkTarget();
	}

	addEntry(program);
	writeConfig();
}

void mainWindow::on_delProgram_clicked(){
    auto items = ui->programs->selectedItems();
    if(items.size()){
        for(auto &item: items){
            removeEntry(item);
        }
        writeConfig();
    }
}

void mainWindow::on_programs_doubleClicked(const QModelIndex &index){
	QListWidgetItem *item = ui->programs->item(index.row());
	entryEditor editor(item, this);
	editor.exec();
	writeConfig();
}

void mainWindow::on_actionShowHideWindow_triggered(){
	setVisible(!isVisible());
}

void mainWindow::on_actionExit_triggered(){
	systray.hide();
	close();
}

void mainWindow::on_actionAbout_triggered(){
	QMessageBox::about(this, "About", "Vibrant linux is a program to automatically set "
									  "the color saturation of specific monitors depending "
									  "on what program is current running.\n\nThis program currently "
									  "only works for NVIDIA systems.\n\nVersion: 1.2.5");
}

void mainWindow::on_donate_clicked(){
	QDesktopServices::openUrl(QUrl("https://paypal.me/vibrantlinux"));
}

void mainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason){
	if(reason == QSystemTrayIcon::ActivationReason::Trigger){
		if(!isVisible()){
			show();
		}
	}
}
