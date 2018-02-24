#pragma once
#include "Demo.h"
#include "Plane.h"

enum EPoolTags : unsigned int {
	pocket = 1,
	ball = 2,
	cueball = 4,
	eightball = 8,
	solid_ball = 16,
	striped_ball = 32
};

class PoolGame : public Demo {
public:
	static const float k_table_width;
	static const float k_table_height;
	static const glm::vec4 k_felt_colour;

	PoolGame();

	virtual void update(float deltaTime, Application2D* app);

	virtual void draw(Application2D* app);

	virtual void setup();
protected:
};