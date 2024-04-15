#include <QSlider>
#include <QSpinBox>
#include <utility>

#include "displaytab.h"

DisplayTab::DisplayTab(QString name, int initialSaturation, QWidget *parent)
    : QWidget(parent), ui(new Ui::DisplayTab), name(std::move(name)) {
  ui->setupUi(this);
  setSaturation(initialSaturation);

  connect(ui->saturationSlider, &QSlider::valueChanged, this,
          &DisplayTab::saturationChanged);
  connect(ui->saturationSpinBox, &QSpinBox::valueChanged, this,
          &DisplayTab::saturationChanged);
}

auto DisplayTab::getSaturation() -> int {
  return ui->saturationSlider->value();
}

void DisplayTab::setSaturation(int saturation) {
  ui->saturationSlider->setValue(saturation);
  ui->saturationSpinBox->setValue(saturation);
}

void DisplayTab::saturationChanged(int value) {
  setSaturation(value);
  emit onSaturationChange(name, value);
}
