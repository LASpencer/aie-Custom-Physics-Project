#include "catch.hpp"

#include "Joint.h"
#include "Spring.h"
#include "Sphere.h"
#include "PhysicsScene.h"

#include "Utility.h"

using namespace physics;

TEST_CASE("Adding ends to spring", "[joint],[spring]") {
	SpringPtr spring(new Spring(3, 2, 0));
	SpherePtr s1(new Sphere({ 3,-4 },1, { 0,0 }, 0, 1));
	SpherePtr s2(new Sphere({ -3,4 },1, { 0,0 }, 0, 2.5f));
	spring->setEnd1(s1);
	REQUIRE(spring->getEnd1() == s1);
	spring->setEnd2(s1);
	// Ends can't be same object
	REQUIRE_FALSE(spring->getEnd2());
	spring->setEnd2(s2);
	REQUIRE(spring->getEnd2() == s2);
	spring->setEnd1(s2);
	// Invalid change to end doesn't remove previous end
	REQUIRE(spring->getEnd1() == s1);
}

TEST_CASE("Removing ends from spring", "[joint],[spring]") {
	SpherePtr s1(new Sphere({ 3,-4 },1, { 0,0 }, 0, 1));
	SpherePtr s2(new Sphere({ -3,4 },1, { 0,0 }, 0, 2.5f));
	SpringPtr spring(new Spring(5, 8, 0, s1, s2));
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,0 });
	SECTION("Kill object to remove") {
		s1->kill();
		spring->earlyUpdate(scene);
		REQUIRE_FALSE(spring->getEnd1());
	}
	delete scene;
}

TEST_CASE("Getters and Setters", "[joint],[spring]") {
	//TODO setting invalid values throws exception
}

TEST_CASE("Calculate energy", "[joint],[spring]") {
	SpherePtr s1(new Sphere({ 3,0 },1, { 0,0 }, 0, 1));
	SpherePtr s2(new Sphere({ 0,4 },1, { 0,0 }, 0, 2.5f));
	SpringPtr spring(new Spring(5, 7, 0, s1));
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,-10 });
	SECTION("Spring with missing end has no energy") {
		REQUIRE(spring->calculateEnergy(scene) == 0);
	}
	SECTION("Energy calculated correctly") {
		spring->setEnd2(s2);
		REQUIRE(spring->calculateEnergy(scene) == Approx(10.f));
	}
	delete scene;
}

TEST_CASE("Spring forces", "[joint],spring]") {
	SpherePtr s1(new Sphere({ 3,-4 },1, { 0,0 },0,1));
	SpherePtr s2(new Sphere({ -3,4 },1, { 0,0 }, 0,2.5f));
	SpringPtr spring(new Spring(5, 8, 0, s1,s2));
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,0 });
	SECTION("Spring restoring force") {
		spring->earlyUpdate(scene);
		s1->fixedUpdate(scene);
		s2->fixedUpdate(scene);
		// Distance = 10, equilibrium = 8, k = 5 so force = 10
		REQUIRE(vectorApprox(s1->getVelocity(), { -0.6f, 0.8f }, k_margin));
		REQUIRE(vectorApprox(s2->getVelocity(), { 0.24f, -0.32f }, k_margin));
	}
	SECTION("Spring damping force") {
		spring->setDamping(2);
		s2->setVelocity({ 0.6f,-0.8f });
		spring->earlyUpdate(scene);
		s1->fixedUpdate(scene);
		s2->fixedUpdate(scene);
		// Total damping force of 2
		REQUIRE(vectorApprox(s1->getVelocity(), { -0.48f, 0.64f }, k_margin));
	}
	SECTION("Revolving objects") {
		spring->setAnchor1({1, 0});
		spring->setAnchor2({ 1,0 }); 
		// Direction = {-0.6f,0.8f}, perpendicular = {-0.8f, -0.6f}
		spring->setDamping(2);
		SECTION("revolving towards") {
			s1->setAngularVelocity(1);
			// Velocity = 0.8, damping force 1.6

			spring->earlyUpdate(scene);
			s1->fixedUpdate(scene);
			s2->fixedUpdate(scene);
			REQUIRE(vectorApprox(s1->getVelocity(), { -0.504f, 0.672f }, k_margin));

		}
	}
	delete scene;
}