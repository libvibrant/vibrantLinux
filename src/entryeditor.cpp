#include "entryeditor.h"
#include "ui_entryeditor.h"

entryEditor::entryEditor(QListWidgetItem* entry, QWidget *parent) : QDialog(parent),
ui(new Ui::entryEditor), entry(entry), info(entry->data(Qt::UserRole).value<programInfo*>()){
	ui->setupUi(this);

	ui->path->setText(info->path);
	for(auto it = info->saturationVals.begin(); it != info->saturationVals.end(); it++){
		auto *dpyTab = new (std::nothrow) displayTab(it.key(), ui->displays);
		dpyTab->setSaturation(it.value());
		ui->displays->addTab(dpyTab, it.key());
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

	auto fileInfo = QFileInfo(program);
	if(fileInfo.isSymLink()){
		program = fileInfo.symLinkTarget();
	}

	ui->path->setText(program);
}

void entryEditor::on_buttonBox_accepted(){
	info->path = ui->path->text();
	entry->setText(programInfo::exeNameFromPath(ui->path->text()));

	for(int i = 0; i < ui->displays->count(); i++){
		displayTab *dpyTab = dynamic_cast<displayTab*>(ui->displays->widget(i));
		info->saturationVals[dpyTab->getName()] = dpyTab->getSaturation();
	}
}
