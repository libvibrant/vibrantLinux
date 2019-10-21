#include "entryeditor.h"
#include "ui_entryeditor.h"

entryEditor::entryEditor(QListWidgetItem *entry, QWidget *parent) : QDialog(parent),
ui(new Ui::entryEditor), entry(entry){
	ui->setupUi(this);

	entryInfo = getItemInfo(entry);

	ui->path->setText(entryInfo->path);

	QStringList displayNames = displayTab::getDisplayNames();

	for(auto &dpy: displayNames){
		displayTab *dpyTab = new (std::nothrow) displayTab(dpy, ui->displays, false);
		dpyTab->setDefaultVibrance((entryInfo->vibranceVals[dpy]));
		ui->displays->addTab(dpyTab, dpy);
	}
}

entryEditor::~entryEditor(){
	delete ui;
}

void entryEditor::on_pathSelect_clicked(){
	QString program = QFileDialog::getOpenFileName(this, tr("Select a program"), QDir::homePath(),
		"Executable (*)", nullptr);
	if(program.isNull()){
		return;
	}
	ui->path->setText(program);
}

void entryEditor::on_buttonBox_accepted(){
	entryInfo->path = ui->path->text();
	entry->setText(pathToName(ui->path->text()));

	QMap<QString, int> vibranceVals;
	for(int i = 0; i < ui->displays->count(); i++){
		displayTab *dpyTab = dynamic_cast<displayTab*>(ui->displays->widget(i));
		vibranceVals.insert(dpyTab->getName(), dpyTab->getDefaultVibrance());
	}

	entryInfo->vibranceVals = vibranceVals;
}
