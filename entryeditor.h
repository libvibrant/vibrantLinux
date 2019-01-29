#ifndef ENTRYEDITOR_H
#define ENTRYEDITOR_H

#include <QDialog>
#include <QFileDialog>
#include "programentry.h"

namespace Ui {
	class entryEditor;
}

class entryEditor : public QDialog{
	Q_OBJECT

public:
	explicit entryEditor(programEntry *entry, QWidget *parent = nullptr);
	~entryEditor();

private slots:
	void on_pathSelect_clicked();

	void on_buttonBox_accepted();

	void on_vibranceSldr_valueChanged(int value);

	void on_vibranceVal_editingFinished();

private:
	Ui::entryEditor *ui;
	programEntry *entry;
};

#endif // ENTRYEDITOR_H
