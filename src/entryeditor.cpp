#include "entryeditor.h"
#include "ui_entryeditor.h"

entryEditor::entryEditor(programInfo& entry, const QStringList& displayNames, QWidget* parent) : QDialog(parent),
ui(new Ui::entryEditor), entry(entry){
	ui->setupUi(this);

	bool isPath = entry.type == programInfo::MatchPath;
	ui->path->setEnabled(isPath);
	ui->pathSelectBt->setEnabled(isPath);

	ui->titleMatch->setEnabled(!isPath);
	ui->titleMatchTypeCb->setEnabled(!isPath);

	if(isPath){
		ui->path->setText(entry.matchString);
	}
	else{
		ui->titleMatchRbt->setChecked(true);
		ui->titleMatch->setText(entry.matchString);
		switch(entry.type){
			case programInfo::MatchPath:
				ui->titleMatchTypeCb->setCurrentIndex(0);
				break;
			case programInfo::SubMatchTitle:
				ui->titleMatchTypeCb->setCurrentIndex(1);
				break;
			case programInfo::RegexMatchTitle:
				ui->titleMatchTypeCb->setCurrentIndex(2);
				break;
		}
		ui->titleMatchTypeCb->currentIndex();
	}

	for(auto& name: displayNames){
		auto *dpyTab = new (std::nothrow) displayTab(name, ui->displays);
		if(dpyTab == nullptr){
			throw std::runtime_error("failed to create display tab");
		}

		dpyTab->setSaturation(entry.saturationVals[name]);
		ui->displays->addTab(dpyTab, name);
	}
}

entryEditor::~entryEditor(){
	delete ui;
}

void entryEditor::on_pathSelectBt_clicked(){
	QString program = QFileDialog::getOpenFileName(this, "Select a program", QDir::homePath(),
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

void entryEditor::on_titleMatchTypeCb_currentIndexChanged(int index){
	switch(index){
	}
}

void entryEditor::on_pathMatchRbt_clicked(){
	ui->path->setEnabled(true);
	ui->pathSelectBt->setEnabled(true);

	ui->titleMatch->setEnabled(false);
	ui->titleMatchTypeCb->setEnabled(false);
}

void entryEditor::on_titleMatchRbt_clicked(){
	ui->path->setEnabled(false);
	ui->pathSelectBt->setEnabled(false);

	ui->titleMatch->setEnabled(true);
	ui->titleMatchTypeCb->setEnabled(true);
}

void entryEditor::accept(){
	//its at times like these that I wish i'd written this in rust all those years ago
	if(ui->pathMatchRbt->isChecked()){
		auto path = ui->path->text();
		if(!QFile::exists(path)){
			QMessageBox::warning(this, "Warning", "Invalid input, path doesn't exist.");
			return;
		}

		auto fileInfo = QFileInfo(path);
		if(fileInfo.isSymLink()){
			path = fileInfo.symLinkTarget();
		}

		entry.type = programInfo::MatchPath;
		entry.matchString = path.toUtf8();
	}
	else{
		switch(ui->titleMatchTypeCb->currentIndex()){
			case 0:
				entry.type = programInfo::MatchTitle;
				break;
			case 1:
				entry.type = programInfo::SubMatchTitle;
				break;
			case 2:
				entry.type = programInfo::RegexMatchTitle;
				break;
		}

		entry.matchString = ui->titleMatch->text().toUtf8();
	}

	for(int i = 0; i < ui->displays->count(); i++){
		displayTab *dpyTab = dynamic_cast<displayTab*>(ui->displays->widget(i));
		entry.saturationVals[dpyTab->getName()] = dpyTab->getSaturation();
	}

	QDialog::accept();
}
