#include "entryeditor.h"
#include "ui_entryeditor.h"

entryEditor::entryEditor(QListWidgetItem *entry, QWidget *parent) : QDialog(parent), ui(new Ui::entryEditor), entry(entry){
	if(entry == nullptr){
		throw std::runtime_error("entryEditor was given a nullptr in constructor");
	}
	ui->setupUi(this);

	ui->path->setText(itemPath(entry));
	displays = displayTab::getDisplays();
	for(auto &dpy: displays){
		dpy.setVibrance(itemVibrance(entry, dpy.getName()));
		ui->displays->addTab((QWidget*)dpy, dpy.getName());
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
	setItemPath(entry, ui->path->text());
	QMap<QString, int> map;
	for(auto &dpy: displays){
		map.insert(dpy.getName(), dpy.getVibrance());
	}
	setItemMap(entry, map);
}
