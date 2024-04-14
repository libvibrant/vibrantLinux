#include "displaymanager.h"

displayManager::displayManager() : scanner(true) {
  auto err = vibrant_instance_new(&instance, nullptr);
  switch (err) {
  case vibrant_NoError:
    break;
  case vibrant_ConnectToX:
    throw std::runtime_error("failed to connect to default X server");
  case vibrant_NoMem:
    throw std::runtime_error("failed to allocate memory for vibrant instance");
  }

  vibrant_instance_get_controllers(instance, &controllers_arr,
                                   &controllers_size);

  for (size_t i = 0; i < controllers_size; i++) {
    auto name = controllers_arr[i].info->name;

    displays.append(name);
    controllers.insert(name, (controller){controllers_arr + i, 100});
  }
}

displayManager::~displayManager() {}

QStringList displayManager::getDisplayNames() { return displays; }

int displayManager::getDisplaySaturation(const QString &name) {
  return vibrant_controller_get_saturation(controllers[name].v_controller) *
         100;
}

void displayManager::updateSaturation(QListWidget *watchlist) {
  auto info = scanner.getSaturation(watchlist);
  if (info != nullptr) {
    for (auto it = info->saturationVals.begin();
         it != info->saturationVals.end(); it++) {
      vibrant_controller_set_saturation(controllers[it.key()].v_controller,
                                        (double)it.value() / 100);
    }
  } else {
    for (auto controller : controllers) {
      vibrant_controller_set_saturation(
          controller.v_controller, (double)controller.defaultSaturation / 100);
    }
  }
}

void displayManager::checkWindowFocus(bool use) {
  scanner.setCheckWindowFocus(use);
}

bool displayManager::isCheckingWindowFocus() {
  return scanner.isCheckingWindowFocus();
}

void displayManager::setDefaultDisplaySaturation(const QString &name,
                                                 int value) {
  controllers[name].defaultSaturation = value;
}
