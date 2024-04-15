#include "entryeditor.h"
#include "ui_entryeditor.h"

EntryEditor::EntryEditor(ProgramInfo &entry, const QStringList &displayNames,
                         QWidget *parent)
    : QDialog(parent), ui(new Ui::EntryEditor), entry(entry) {
  ui->setupUi(this);

  bool isPath = entry.type == ProgramInfo::MatchPath;
  ui->path->setEnabled(isPath);
  ui->pathSelectBt->setEnabled(isPath);

  ui->titleMatch->setEnabled(!isPath);
  ui->titleMatchTypeCb->setEnabled(!isPath);

  if (isPath) {
    ui->path->setText(entry.matchString);
  } else {
    ui->titleMatchRbt->setChecked(true);
    ui->titleMatch->setText(entry.matchString);
    switch (entry.type) {
    case ProgramInfo::MatchPath:
      ui->titleMatchTypeCb->setCurrentIndex(0);
      break;
    case ProgramInfo::SubMatchTitle:
      ui->titleMatchTypeCb->setCurrentIndex(1);
      break;
    case ProgramInfo::RegexMatchTitle:
      ui->titleMatchTypeCb->setCurrentIndex(2);
      break;
    }
    ui->titleMatchTypeCb->currentIndex();
  }

  for (auto &name : displayNames) {
    auto *dpyTab = new (std::nothrow)
        DisplayTab(name, entry.saturationVals[name], ui->displays);
    if (dpyTab == nullptr) {
      throw std::runtime_error("failed to create display tab");
    }

    ui->displays->addTab(dpyTab, name);
  }
}

EntryEditor::~EntryEditor() { delete ui; }

void EntryEditor::on_pathSelectBt_clicked() {
  QString program = QFileDialog::getOpenFileName(
      this, "Select a program", QDir::homePath(), "Executable (*)", nullptr);
  if (program.isNull()) {
    return;
  }

  auto fileInfo = QFileInfo(program);
  if (fileInfo.isSymLink()) {
    program = fileInfo.symLinkTarget();
  }

  ui->path->setText(program);
}

void EntryEditor::on_titleMatchTypeCb_currentIndexChanged(int index) {
  switch (index) {}
}

void EntryEditor::on_pathMatchRbt_clicked() {
  ui->path->setEnabled(true);
  ui->pathSelectBt->setEnabled(true);

  ui->titleMatch->setEnabled(false);
  ui->titleMatchTypeCb->setEnabled(false);
}

void EntryEditor::on_titleMatchRbt_clicked() {
  ui->path->setEnabled(false);
  ui->pathSelectBt->setEnabled(false);

  ui->titleMatch->setEnabled(true);
  ui->titleMatchTypeCb->setEnabled(true);
}

void EntryEditor::accept() {
  // its at times like these that I wish i'd written this in rust all those
  // years ago
  if (ui->pathMatchRbt->isChecked()) {
    auto path = ui->path->text();
    if (!QFile::exists(path)) {
      QMessageBox::warning(this, "Warning",
                           "Invalid input, path doesn't exist.");
      return;
    }

    auto fileInfo = QFileInfo(path);
    if (fileInfo.isSymLink()) {
      path = fileInfo.symLinkTarget();
    }

    entry.type = ProgramInfo::MatchPath;
    entry.matchString = path.toUtf8();
  } else {
    switch (ui->titleMatchTypeCb->currentIndex()) {
    case 0:
      entry.type = ProgramInfo::MatchTitle;
      break;
    case 1:
      entry.type = ProgramInfo::SubMatchTitle;
      break;
    case 2:
      entry.type = ProgramInfo::RegexMatchTitle;
      break;
    }

    entry.matchString = ui->titleMatch->text().toUtf8();
  }

  for (int i = 0; i < ui->displays->count(); i++) {
    DisplayTab *dpyTab = dynamic_cast<DisplayTab *>(ui->displays->widget(i));
    entry.saturationVals[dpyTab->name] = dpyTab->getSaturation();
  }

  QDialog::accept();
}
