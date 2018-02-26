#pragma once
#include "Demo.h"

class TitleScreen : public Demo {
public:
	TitleScreen();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();
};