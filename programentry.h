#ifndef PROGRAMENTRY_H
#define PROGRAMENTRY_H

#include <QListWidget>

class programEntry{
public:
	programEntry(QListWidgetItem *item, int vibrance = 0);
	programEntry(programEntry &other) = delete;
	programEntry(programEntry &&other) noexcept;
	~programEntry();

	programEntry &operator=(programEntry &&other){
		if(this == &other){
			return *this;
		}

		item = other.item;
		path = other.path;
		vibrance = other.vibrance;
		other.item = nullptr;

		return *this;
	}

	bool operator==(programEntry &other){
		return path == other.path;
	}
	bool operator==(programEntry &&other){
		return path == other.path;
	}
	bool operator==(QString path){
		return this->path == path;
	}
	bool operator==(QListWidgetItem *item){
		return this->item == item;
	}

	QString toJson();

public:
	QString path;
	int vibrance;
private:
	QListWidgetItem *item = nullptr;
};

#endif // PROGRAMENTRY_H
