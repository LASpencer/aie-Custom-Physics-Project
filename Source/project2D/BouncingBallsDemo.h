#pragma once
#include "Demo.h"
#include "ExternalLibraries.h"

class Application2D;

class BouncingBallsDemo : public Demo {
public:
	static const float k_stick_force_multiplier;
	static const float k_stick_max_force;

	BouncingBallsDemo();

	~BouncingBallsDemo();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();

protected:
	physics::PhysicsScene* m_scene;

	glm::vec2 m_cueContact;		// Point where cue will strike
	bool m_cueActive;
	bool m_gravityOn;

	void shootCue(Application2D* app);
};