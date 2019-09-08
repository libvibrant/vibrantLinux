#include "utils.h"

bool systemHasNvidia(){
	for(size_t i = 0;; i++){
		char *realPath = realpath(std::string("/sys/class/drm/card"+std::to_string(i)+"/device/driver").c_str(), nullptr);
		if(!realPath){
			break;
		}
		std::string driver(realPath);
		free(realPath);

		for(char &c: driver){
			c = static_cast<char>(std::tolower(c));
		}

		if(driver.find("nvidia") != std::string::npos){
			return true;
		}
	}
	return false;
}

QString programName(QString path){
	return path.split("/").back();
}

int itemVibrance(const QListWidgetItem *item, QString key){
	return itemMap(item)[key];
}

void setItemVibrance(QListWidgetItem *item, QString key, int val){
	QMap<QString, int> map = itemMap(item);
	map[key] = val;
	setItemMap(item, map);
}

QMap<QString, int> itemMap(const QListWidgetItem *item){
	return item->data(Qt::ItemDataRole::UserRole).value<QMap<QString, int>>();
}

void setItemMap(QListWidgetItem *item, QMap<QString, int> map){
	item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(map));
}

QString itemPath(const QListWidgetItem *item){
	return item->data(Qt::ItemDataRole::AccessibleDescriptionRole).value<QString>();
}

void setItemPath(QListWidgetItem *item, QString path){
	item->setText(programName(path));
	item->setData(Qt::ItemDataRole::AccessibleDescriptionRole, QVariant::fromValue(path));
}

QString itemToJson(const QListWidgetItem *item){
	QString json = "{\"path\": \"" + item->data(Qt::ItemDataRole::AccessibleDescriptionRole).toString() + "\",";
	json += "\"vibrance\": [";

	QMap<QString, int> vibranceMap = itemMap(item);
	for(auto vibrance = vibranceMap.begin(); vibrance != vibranceMap.end();){
		json += "{\"name\": \"" + vibrance.key() + "\",";
		json += "\"vibrance\": " + QString::number(vibrance.value()) + '}';
		if(++vibrance != vibranceMap.end()){
			json += ',';
		}
	}

	json += "]}";
	return json;
}
