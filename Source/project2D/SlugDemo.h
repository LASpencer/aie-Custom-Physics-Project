#pragma once
#include "Demo.h"
//TODO demo where you play as a slug moving through maze/obstacle course
// Player controls head, which is attached to a softbody through springs
// Make the player squeeze through holes, shoot cannonballs at them
// Maybe can make softbody more or less viscous
// Try side-on

class Slug;

class SlugDemo : public Demo {
public:

	static const float k_box_elasticity;
	static const float k_box_friction;

	static const glm::vec2 k_slug_start;

	SlugDemo();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();

private:
	physics::PhysicsScene* m_scene;

	std::shared_ptr<Slug> m_slug;
};