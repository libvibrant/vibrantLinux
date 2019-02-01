#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>
#include <string>

#include <QProcess>

#include "device.h"

enum class displayType{
	nvidia,
	amd,
	intel
};

class display{
public:
	display();
	display(displayType type, std::string displayName);
	display(const display &other) noexcept;
	display(display &&other) noexcept;
	~display();

	display &operator=(const display &other){
		if(this == &other){
			return *this;
		}

		type = other.type;
		displayName = other.displayName;
		return *this;
	}

	display &operator=(display &&other){
		if(this == &other){
			return *this;
		}

		type = other.type;
		displayName = std::move(other.displayName);
		return *this;
	}

	void applyVibrance(int vibrance);

	static std::vector<display> getDisplays();

	displayType type;
	std::string displayName;
};

#endif // DISPLAY_H
