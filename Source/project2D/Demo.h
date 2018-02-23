#pragma once
#include "PhysicsScene.h"

class Application2D;

class Demo {
public:

	virtual void update(float deltaTime, Application2D* app) = 0;

	virtual void draw(Application2D* app) = 0;

	virtual void setup() = 0;

protected:
	physics::PhysicsScene * m_scene;
};