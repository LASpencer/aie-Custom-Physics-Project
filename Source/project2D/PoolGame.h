#pragma once
#include "Sphere.h"

#include "Demo.h"

enum EPoolTags : unsigned int {
	pocket = 1,
	ball = 2,
	cueball = 4,
	eightball = 8,
	solid_ball = 16,
	striped_ball = 32
};

class PoolBall;
typedef std::shared_ptr<PoolBall> PoolBallPtr;
typedef std::weak_ptr<PoolBall> PoolBallWeakPtr;

class PoolGame : public Demo {
public:
	static const float k_table_width;
	static const float k_table_height;
	static const float k_rail_friction;
	static const float k_rail_elasticity;
	static const float k_stick_force_multiplier;
	static const float k_stick_max_force;
	static const glm::vec4 k_felt_colour;

	PoolGame();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);
protected:
	std::vector<PoolBallPtr> m_balls;

	glm::vec2 m_cueContact;
	bool m_cueActive;

	virtual void setup();
	virtual void rack();

	void shootCue(physics::Sphere* cue, Application2D* app);
};