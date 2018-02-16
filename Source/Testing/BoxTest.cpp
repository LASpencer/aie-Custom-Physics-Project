#include "catch.hpp"

#include "Box.h"
#include "Sphere.h"
#include "Plane.h"

#include "Utility.h"

using namespace physics;

TEST_CASE("Box Constructor") {
	// Mass must be positive
	REQUIRE_THROWS(Box(glm::vec2(0,0), 1, 1, 0, glm::vec2(0,0), 0, -1));
	// Width and height must be positive and non zero
	REQUIRE_THROWS(Box(glm::vec2(0, 0), 0, 1, 0));
	REQUIRE_THROWS(Box(glm::vec2(0, 0), -1, 1, 0));
	REQUIRE_THROWS(Box(glm::vec2(0, 0), 1, 0, 0));
	REQUIRE_THROWS(Box(glm::vec2(0, 0), 1, -1, 0));
}

TEST_CASE("Box Getters and Setters") {
	Box* b = new Box({ 0,0 }, 2, 3, 0);
	SECTION("Setting height and width") {
		REQUIRE(b->getWidth() == Approx(2));
		REQUIRE(b->getHeight() == Approx(3));
		REQUIRE(vectorApprox(b->getXExtent(), { 1,0 }));
		REQUIRE(vectorApprox(b->getYExtent(), { 0,1.5f }));
		// Cannot pass zero or negative
		REQUIRE_THROWS(b->setWidth(0));
		REQUIRE_THROWS(b->setHeight(0));
		b->setWidth(3.2f);
		b->setHeight(1.7f);
		REQUIRE(b->getWidth() == Approx(3.2f));
		REQUIRE(b->getHeight() == Approx(1.7f));
		REQUIRE(vectorApprox(b->getXExtent(), { 1.6f,0 }));
		REQUIRE(vectorApprox(b->getYExtent(), { 0,0.85f }));
	}
	//TODO get/set width (fails if not positive), changes xExtent
	//TODO get/set height (fails if not positive), changes yExtent
	SECTION("Changing orientation alters extents") {
		b->setOrientation(0.5236f);
		REQUIRE(vectorApprox(b->getXExtent(), { 0.866f,0.5f }, 0.001f));
		REQUIRE(vectorApprox(b->getYExtent(), { -0.75f,1.299f }, 0.001f));
		b->setOrientation(-1);
		REQUIRE(vectorApprox(b->getXExtent(), { 0.541f,-0.842f }, 0.001f));
		REQUIRE(vectorApprox(b->getYExtent(), { 1.262f,0.811f }, 0.001f));

		REQUIRE(b->getWidth() == Approx(2));
		REQUIRE(b->getHeight() == Approx(3));
	}
	delete b;
}

TEST_CASE("Get Corners") {
	Box* b = new Box({ 0,0 }, 2, 3, 0);
	std::array<glm::vec2, 4> corners = b->getCorners();
	//TODO with different orientation, get corners
	SECTION("Corners given in ccw winding") {
		REQUIRE(vectorApprox(corners[0], { -1,-1.5f }));
		REQUIRE(vectorApprox(corners[1], { 1,-1.5f }));
		REQUIRE(vectorApprox(corners[2], { 1,1.5f }));
		REQUIRE(vectorApprox(corners[3], { -1,1.5f }));
		// Move box to change corners
		b->setPosition({ 3,2 });
		corners = b->getCorners();
		REQUIRE(vectorApprox(corners[0], { 2,0.5f }));
		REQUIRE(vectorApprox(corners[1], { 4,0.5f }));
		REQUIRE(vectorApprox(corners[2], { 4,3.5f }));
		REQUIRE(vectorApprox(corners[3], { 2,3.5f }));
	}
	SECTION("Changing orientation") {
		b->setOrientation(0.5236f);
		corners = b->getCorners();
		REQUIRE(vectorApprox(corners[0], { -0.116f, -1.799f}, 0.005f));
		REQUIRE(vectorApprox(corners[1], { 1.616f,-0.799f },0.005f));
		REQUIRE(vectorApprox(corners[2], { 0.116f,1.799f},0.005f));
		REQUIRE(vectorApprox(corners[3], { -1.616f,0.799f },0.005f));
	}
	delete b;
}

TEST_CASE("Local to world coordinate conversion") {
	Box* b = new Box({ 0,0 }, 2, 3, 0);
	b->setPosition({8, -2});
	REQUIRE(vectorApprox(b->worldToLocalSpace({ 12,9 }), { 4,11 }));
	REQUIRE(vectorApprox(b->localToWorldSpace({ 7,4 }), { 15,2 }));
	// Conversion after rotating
	b->setOrientation(0.5236f);
	REQUIRE(vectorApprox(b->worldToLocalSpace({ 12,9 }), { 8.964f,7.526f},0.005f));
	REQUIRE(vectorApprox(b->localToWorldSpace({ 7,4 }), { 12.062f, 4.964f },0.005f));
	delete b;
}

TEST_CASE("Box-Box collision") {
	//TODO collision tests
	Box* b1 = new Box({ 0,0 }, 4, 3, 0);
	Box* b2 = new Box({ 0,4 }, 3, 4, 0);

	// Miss
	Collision col = b1->checkBoxCollision(b2);
	
	REQUIRE_FALSE(col);

	// Edge to edge
	b2->setPosition({ 0,3 });
	col = b1->checkBoxCollision(b2);
	if (col.first != b1) {
		col = col.reverse();
	}

	REQUIRE(col);
	REQUIRE(vectorApprox(col.normal, { 0,-1 }));
	REQUIRE(col.depth == Approx(0.5f));
	REQUIRE(vectorApprox(col.contact, { 0,1.25f }, k_margin));

	// TODO corner to edge
	b2->setOrientation(0.5236f);
	col = b1->checkBoxCollision(b2);
	if (col.first != b1) {
		col = col.reverse();
	}

	REQUIRE(col);
	REQUIRE(vectorApprox(col.normal, { 0,-1 }));
	REQUIRE(col.depth == Approx(0.982f).margin(0.001f));
	REQUIRE(vectorApprox(col.contact, { 0.079f,1.173f }, 0.001f));

	delete b1;
	delete b2;
}

TEST_CASE("Box-Sphere collision") {
	Box* b = new Box({ 0,0 }, 4, 3, 0);
	Sphere* s = new Sphere({ 3,3 }, { 0,0 }, 1);
	Collision col = b->checkSphereCollision(s);
	REQUIRE_FALSE(col);
	s->setPosition({ 2.5f,2 });
	col = b->checkSphereCollision(s);
	REQUIRE(col);
	REQUIRE(vectorApprox(col.normal, { -0.7809f,-0.6247f }, 0.001f));
	REQUIRE(col.depth == Approx(0.297f).margin(0.005f));
	REQUIRE(vectorApprox(col.contact, { 1.835f,1.468f }, 0.005f));

	delete b;
	delete s;
}

TEST_CASE("Box-Plane collision") {
	Box* b = new Box({ 0,0 }, 4, 3, 0);
	Plane* p = new Plane({1,0},3);
	Collision col = b->checkPlaneCollision(p);
	REQUIRE_FALSE(col);
	p->setDistance(1);
	col = b->checkPlaneCollision(p);
	REQUIRE(col);
	REQUIRE(vectorApprox(col.normal, { 1,0 }));
	REQUIRE(col.depth == Approx(1));
	delete b;
	delete p;
}

TEST_CASE("Moment of Inertia") {
	//calculate moment of inertia after changing mass, width, height
	Box* b = new Box({ 0,0 }, 2, 3, 0);
	SECTION("Moment of dynamic box") {
		REQUIRE(b->getMoment() == Approx(0.5f));
		REQUIRE(b->getInvMoment() == Approx(2.f));
		b->setMass(4);
		REQUIRE(b->getMoment() == Approx(2.f));
		REQUIRE(b->getInvMoment() == Approx(0.5f));
		b->setWidth(5);
		REQUIRE(b->getMoment() == Approx(5.f));
		REQUIRE(b->getInvMoment() == Approx(0.2f));
		b->setHeight(6);
		REQUIRE(b->getMoment() == Approx(10.f));
		REQUIRE(b->getInvMoment() == Approx(0.1f));
	}
	SECTION("Moment of static box") {
		b->setMass(0);
		REQUIRE(b->getMoment() == INFINITY);
		REQUIRE(b->getInvMoment() == 0.f);

		b->setMass(1);
		REQUIRE(b->getMoment() == Approx(0.5f));

		b->setStatic(true);
		REQUIRE(b->getMoment() == INFINITY);
		REQUIRE(b->getInvMoment() == 0.f);
	}
	delete b;
}

TEST_CASE("Find best edge") {
	//TODO for different vectors, return edge (ccw winding) closest to it
	Box* b = new Box({ 0,0 }, 2, 3, 0.5236f);
	Edge e = b->findBestEdge({ 1,0 });
	REQUIRE(vectorApprox(e.start, { 1.616f,-0.799f }, 0.005f));
	REQUIRE(vectorApprox(e.end, { 0.116f,1.799f }, 0.005f));
	REQUIRE(vectorApprox(e.direction, { -0.5f,0.866f }, 0.005f));
	REQUIRE(e.getLength() == Approx(3));
	REQUIRE(vectorApprox(e.getNormal(), {0.866f, 0.5f},0.005f));
	delete b;
}
TEST_CASE("Clipping edges") {
	Edge e1 = { {3,0},{10,0},{1,0} };
	Edge e2 = { {12,3},{5,3},{-1,0} };
	e1.clip({ -1,0 }, 12);	// No change
	REQUIRE(e1.start.x == Approx(3));
	REQUIRE(e1.end.x == Approx(10));
	e1.clip({ 1,0 }, 5);	// cuts off start
	REQUIRE(e1.start.x == Approx(5));
	REQUIRE(e1.end.x == Approx(10));
	// TODO test oblique case
	REQUIRE(false);
}

TEST_CASE("Edge intersection") {
	//TODO test intersection checks
	Edge e1 = { {2,3},{4,8},glm::normalize(glm::vec2(2,5)) };
	Edge e2 = { {5,2}, {3,6}, glm::normalize(glm::vec2(-2,4)) };
	glm::vec2 intersection;
	REQUIRE(e1.checkIntersection(e2, intersection));
	REQUIRE(vectorApprox(intersection, {3.111f,5.778f}, 0.001f));
	// in same direction, but too short
	e2.end = { 3.5f,5 };
	REQUIRE_FALSE(e1.checkIntersection(e2, intersection));
	// parallel case
	e2 = { { 3,4 },{ 5,9 },glm::normalize(glm::vec2(2,5)) };
	REQUIRE_FALSE(e1.checkIntersection(e2, intersection));
}
