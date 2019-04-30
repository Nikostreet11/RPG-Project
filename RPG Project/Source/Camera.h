/*
 * View.h
 *
 *  Created on: Jul 26, 2018
 *      Author: nicop
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "directionsEnum.h"

class Camera {
public:
	Camera(int width, int height, int defaultShiftValue = 1);
	virtual ~Camera();

	void shift(Direction direction);
	void shift(Direction direction, int shiftValue);
	/*void shiftRight();
	void shiftLeft();
	void shiftDown();
	void shiftUp();*/

	int getHeight() const;
	int getWidth() const;
	int getX() const;
	int getY() const;
	void setDefaultShiftValue(int defaultShiftValue);
	void setX(int x);
	void setY(int y);

private:
	int x;
	int y;
	int width;
	int height;
	int defaultShiftValue;
};

#endif /* CAMERA_H_ */
