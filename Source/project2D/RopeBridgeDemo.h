#pragma once
#include "Demo.h"

#include "ExternalLibraries.h"
#include "RigidBody.h"
#include "Rope.h"

class Application2D;

class RopeBridgeDemo : public Demo {
public:
	static const float k_pull_force_multiplier;
	static const float k_max_pull_force;
	static const float k_def_tightness;
	static const float k_max_tightness;
	static const float k_min_tightness;

	RopeBridgeDemo();

	~RopeBridgeDemo();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();

protected:
	physics::PhysicsScene* m_scene;

	physics::RigidBodyPtr m_selectedObject;

	physics::Rope m_suspensionRope;
	physics::SpringPtr m_anchorSpring1;
	physics::SpringPtr m_anchorSpring2;

	float m_ropeTightness;
};