#include "entryeditor.h"
#include "ui_entryeditor.h"

entryEditor::entryEditor(programEntry *entry, QWidget *parent) : QDialog(parent), ui(new Ui::entryEditor), entry(entry){
	if(entry == nullptr){
		throw std::runtime_error("entryEditor was given a nullptr in constructor");
	}

	ui->setupUi(this);
	ui->vibranceVal->setValidator(new QIntValidator(-1024, 1023, this));

	ui->path->setText(entry->path);
	ui->vibranceSldr->setValue(entry->vibrance);
	ui->vibranceVal->setText(QString::number(entry->vibrance));
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
	entry->path = ui->path->text();
	entry->vibrance = ui->vibranceSldr->value();
}

void entryEditor::on_vibranceSldr_valueChanged(int value){
	ui->vibranceVal->setText(QString::number(value));
}

void entryEditor::on_vibranceVal_editingFinished(){
	ui->vibranceSldr->setValue(ui->vibranceVal->text().toInt());
}
