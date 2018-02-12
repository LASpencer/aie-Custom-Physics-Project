#include "catch.hpp"

#include "PhysicsScene.h"
#include "Sphere.h"
#include "Plane.h"

using namespace physics;
//TODO physics scene tests

//TODO figure out desired behaviour/test cases when adding and removing actors

TEST_CASE("Adding actors to scene", "[physics scene]") {
	PhysicsScene scene;
	SECTION("Adding by pointer") {
		Sphere* s = new Sphere({ 0,0 }, { 0,0 }, 1);
		REQUIRE(scene.addActor(s));
		REQUIRE(scene.inScene(s));
		// Cannot add twice
		REQUIRE_FALSE(scene.addActor(s));
	}
	SECTION("Adding by shared pointer")
	{
		SpherePtr s(new Sphere({ 0,0 }, { 0,0 }, 1));
		REQUIRE(scene.addActor(s));
		REQUIRE(scene.inScene(s));
		REQUIRE(s.use_count() == 2);
		// Cannot add twice
		REQUIRE_FALSE(scene.addActor(s));
	}
}

TEST_CASE("Removing actors from scene", "[physics scene]") {
	PhysicsScene scene;
	SECTION("Removing by pointer") {
		Sphere* s = new Sphere({ 0,0 }, { 0,0 }, 1);
		scene.addActor(s);
		REQUIRE(scene.inScene(s));
		scene.removeActor(s);
		REQUIRE_FALSE(scene.inScene(s));
	}
	SECTION("Removing shared pointer")
	{
		SpherePtr s(new Sphere({ 0,0 }, { 0,0 }, 1));
		scene.addActor(s);
		REQUIRE(scene.inScene(s));
		REQUIRE(s.use_count() == 2);
		SECTION("Remove shared pointer by shared") {
			scene.removeActor(s);
			REQUIRE_FALSE(scene.inScene(s));
			REQUIRE_FALSE(s->isAlive());
		}
		SECTION("Remove shared pointer by contained pointer") {
			scene.removeActor(s.get());
			REQUIRE_FALSE(scene.inScene(s));
			REQUIRE_FALSE(s->isAlive());
		}
	}
	SECTION("Remove by killing") {
		SpherePtr s(new Sphere({ 0,0 }, { 0,0 }, 1));
		scene.addActor(s);
		scene.update(1);
		REQUIRE(scene.inScene(s));
		s->kill();
		scene.update(1);
		REQUIRE_FALSE(scene.inScene(s));
	}
}