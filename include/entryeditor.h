#ifndef ENTRYEDITOR_H
#define ENTRYEDITOR_H

#include <vector>

#include <QDialog>
#include <QFileDialog>
#include <QListWidgetItem>

#include "displaytab.h"
#include "programinfo.h"

namespace Ui {
	class entryEditor;
}

class entryEditor : public QDialog{
	Q_OBJECT

public:
	//the user data for this should point to a programInfo struct
	explicit entryEditor(QListWidgetItem* entry, QWidget *parent = nullptr);
	~entryEditor();

private slots:
	void on_pathSelect_clicked();
	void on_buttonBox_accepted();

private:
	Ui::entryEditor *ui;
	QListWidgetItem *entry;
	programInfo *info;
};

#endif // ENTRYEDITOR_H
