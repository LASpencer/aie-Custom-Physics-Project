#include "catch.hpp"

#include "Joint.h"
#include "Spring.h"
#include "Sphere.h"

#include "Utility.h"

using namespace physics;

TEST_CASE("Adding ends to spring", "[joint],[spring]") {
	//TODO can add ends
	//TODO both ends can't be the same object
}

TEST_CASE("Removing ends from spring", "[joint],[spring]") {
	//TODO removing end
	//TODO killing end leads to its removal
}

TEST_CASE("Getters and Setters", "[joint],[spring]") {
	//TODO setting invalid values throws exception
}

TEST_CASE("Calculate energy", "[joint],[spring]") {
	//TODO spring with a missing end has no energy
	//TODO spring calculates energy correctly
}

TEST_CASE("Spring forces", "[joint],spring]") {
	//TODO spring attached to bodies, call one earlyUpdate/fixedUpdate and check new velocity
	//TODO also a case with damping, with velocity starting at some value
	SpherePtr s1(new Sphere({ 3,-4 }, { 0,0 },1,1));
	SpherePtr s2(new Sphere({ -3,4 }, { 0,0 }, 1,2.5f));
	SpringPtr spring(new Spring(5, 8, 0, s1,s2));
	SECTION("Spring restoring force") {
		spring->earlyUpdate(0.1f);
		s1->fixedUpdate({ 0,0 },0.1f);
		s2->fixedUpdate({ 0,0 },0.1f);
		// Distance = 10, equilibrium = 8, k = 5 so force = 10
		REQUIRE(vectorApprox(s1->getVelocity(), { -0.6f, 0.8f }, k_margin));
		REQUIRE(vectorApprox(s2->getVelocity(), { 0.24f, -0.32f }, k_margin));
	}
	SECTION("Spring damping force") {
		spring->setDamping(2);
		s2->setVelocity({ 0.6f,-0.8f });
		spring->earlyUpdate(0.1f);
		s1->fixedUpdate({ 0,0 }, 0.1f);
		s2->fixedUpdate({ 0,0 }, 0.1f);
		// Total damping force of 2
		REQUIRE(vectorApprox(s1->getVelocity(), { -0.48f, 0.64f }, k_margin));
	}
}