#include "displaymanager.h"

DisplayManager::DisplayManager() : scanner(true) {
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

DisplayManager::~DisplayManager() { vibrant_instance_free(&instance); }

QStringList DisplayManager::getDisplayNames() { return displays; }

int DisplayManager::getDisplaySaturation(const QString &name) {
  return vibrant_controller_get_saturation(controllers[name].v_controller) *
         100;
}

void DisplayManager::updateSaturation(QListWidget *watchlist) {
  auto info = scanner.getSaturation(watchlist);
  if (info != nullptr) {
    for (auto entry : info->saturationVals.asKeyValueRange()) {
      vibrant_controller_set_saturation(controllers[entry.first].v_controller,
                                        (double)entry.second / 100);
    }
  } else {
    for (auto controller : controllers) {
      vibrant_controller_set_saturation(
          controller.v_controller, (double)controller.defaultSaturation / 100);
    }
  }
}

void DisplayManager::checkWindowFocus(bool use) {
  scanner.setCheckWindowFocus(use);
}

bool DisplayManager::isCheckingWindowFocus() {
  return scanner.isCheckingWindowFocus();
}

void DisplayManager::setDefaultDisplaySaturation(const QString &name,
                                                 int value) {
  controllers[name].defaultSaturation = value;
}
