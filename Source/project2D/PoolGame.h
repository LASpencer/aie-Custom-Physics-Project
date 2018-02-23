#pragma once
#include "Demo.h"
#include "Plane.h"

class PoolGame : public Demo {
public:
	PoolGame();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();
protected:
};