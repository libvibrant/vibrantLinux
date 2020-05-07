#ifndef GPUCONTROLLER_H
#define GPUCONTROLLER_H

#include <QStringList>
#include <QHash>

/*
 * Common interface that all gpu controllers should implement/inherit
 * saturation values will all work on a percent scale, ints passed to and from
 * these functions should only be between in the range of [-100, 100].
 * where -100 is zero saturation (black and white), 0 means the saturation isnt being tweaked
 * and 100 is full increasing the saturation.
*/
class displayController{
public:
	/*
	 * The saturation can change without us doing anything, and sometimes we need to get the *actual* saturation
	 * for example: if a there was a call to someDisplayController.setSaturation(100), and after that call some external
	 * factor such user action or another program changes the saturation to 0. It is important your implementation of this
	 * class does not just store the saturation from the last call to setSaturation. getSaturation should make an actual query
	 * and return the real saturation of the system so that in the situation described above it would return a value of 0, not 100
	*/
	virtual int getSaturation()=0;

	/*
	 * No need to validate input in this function. The saturation passed to you
	 * is guaranteed to be in the valid range of [-100, 100]
	*/
	virtual void setSaturation(int saturation)=0;

	/*
	 * Don't worry about this. This is the saturation value that will be used for
	 * the display if none of the programs in the watchlist are running.
	 * Just makes it easier for displayManager if we stored this value along with
	 * displayController
	*/
	int defaultSaturation = 0;
};

#endif // GPUCONTROLLER_H
