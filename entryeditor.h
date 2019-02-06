#ifndef ENTRYEDITOR_H
#define ENTRYEDITOR_H

#include <vector>

#include <QDialog>
#include <QFileDialog>
#include <QListWidgetItem>

#include "displaytab.h"
#include "utils.h"

namespace Ui {
	class entryEditor;
}

class entryEditor : public QDialog{
	Q_OBJECT

public:
	explicit entryEditor(QListWidgetItem *entry, QWidget *parent = nullptr);
	~entryEditor();

private slots:
	void on_pathSelect_clicked();
	void on_buttonBox_accepted();

private:
	Ui::entryEditor *ui;
	QListWidgetItem *entry;
	std::vector<displayTab> displays;
};

#endif // ENTRYEDITOR_H
