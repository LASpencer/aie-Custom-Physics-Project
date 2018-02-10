#include "catch.hpp"

#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"

#include "Utility.h"

// This file contains scenarios to be run in a physics scene
using namespace physics;


TEST_CASE("Two billiard balls", "[simulation]") {
	Sphere* cueBall = new Sphere({ -20, 0 }, { 11.11f,0 }, 1, 0.17f);
	Sphere* eightBall = new Sphere({ 0, 0 }, { 0,0 }, 1, 0.16f);
	PhysicsScene* scene = new PhysicsScene(0.01f, { 0,0 });

	scene->addActor(cueBall);
	scene->addActor(eightBall);
	SECTION("Head on collision") {
		float initialEnergy = cueBall->calculateEnergy({ 0,0 }) + eightBall->calculateEnergy({ 0,0 });
		glm::vec2 initialMomentum = cueBall->calculateMomentum() + eightBall->calculateMomentum();

		glm::vec2 cueFinalVelocity = { 0.336666f,0 };
		glm::vec2 eightFinalVelocity = { 11.446666f,0 };

		// After 2 seconds, spheres should have collided
		scene->update(2);

		float finalEnergy = cueBall->calculateEnergy({ 0,0 }) + eightBall->calculateEnergy({ 0,0 });
		glm::vec2 finalMomentum = cueBall->calculateMomentum() + eightBall->calculateMomentum();

		REQUIRE(finalEnergy == Approx(initialEnergy));
		REQUIRE(vectorApprox(initialMomentum, finalMomentum));
		REQUIRE(vectorApprox(cueFinalVelocity, cueBall->getVelocity(), k_margin));
		REQUIRE(vectorApprox(eightFinalVelocity, eightBall->getVelocity(), k_margin));
	}
	//TODO some kind of oblique collision
}