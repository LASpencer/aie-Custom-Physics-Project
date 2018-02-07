#include "catch.hpp"

#include "Sphere.h"
#include "Plane.h"

using namespace physics;

//TODO testing rigidbody updating, applying forces to bodies, reaction forces, testing collision resolution

// TODO decide whether zero/infinite mass is allowed, and test for that

bool vectorApprox(glm::vec2 v1, glm::vec2 v2, float margin = 0) {
	return (v1.x == Approx(v2.x).margin(margin)) && (v1.y == Approx(v2.y).margin(margin));
}

TEST_CASE("Sphere Constructor", "[rigidbody],[sphere]") {
	SECTION("Mass must be positive") {
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 1, -1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 1, NAN));
	}
	SECTION("Radius must be positive and finite") {
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), 0,1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), -1, 1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), INFINITY, 1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), glm::vec2(0, 0), NAN, 1));
	}
}

TEST_CASE("Sphere getters and setters", "[rigidbody],[sphere]") {
	Sphere s(glm::vec2(1, 2), glm::vec2(3, 4), 5, 6);
	SECTION("Getters") {
		REQUIRE(s.getPosition() == glm::vec2(1, 2));
		REQUIRE(s.getOrientation() == 0);
		REQUIRE(s.getRadius() == 5);
		REQUIRE(s.getMass() == 6);
	}
	SECTION("Setters") {
		SECTION("Mass must be positive") {
			REQUIRE_THROWS(s.setMass(-1));
			REQUIRE_THROWS(s.setMass(NAN));
		}
		s.setMass(3);
		REQUIRE(s.getMass() == 3);

		SECTION("Radius must be positive and finite") {
			REQUIRE_THROWS(s.setRadius(0));
			REQUIRE_THROWS(s.setRadius(-1));
			REQUIRE_THROWS(s.setRadius(INFINITY));
			REQUIRE_THROWS(s.setRadius(NAN));
		}
	}
	

}


TEST_CASE("Sphere-sphere collision", "[rigidbody],[sphere],[collision]") {
	Sphere s1(glm::vec2(0, 0), glm::vec2(0, 0), 5);
	Sphere s2(glm::vec2(10, 0), glm::vec2(0, 0), 5);
	Collision collision;
	SECTION("Collision miss") {
		collision = s1.checkCollision(&s2);
		REQUIRE(collision.success == false);
	}
	SECTION("Collision hit") {
		s2.setPosition({ 4,0 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(vectorApprox(collision.normal, { -1,0 }));

		s2.setPosition({ 0,9 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(vectorApprox(collision.normal, {0,-1 }));

		s2.setRadius(1);
		s2.setPosition({ -1,2 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(vectorApprox(collision.normal, { 0.447, -0.894 }, 0.001));

		s2.setPosition({ 0, 0 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(glm::length(collision.normal) == Approx(1));
	}
}

TEST_CASE("Collision struct reversal", "[collision]") {
	Sphere s1(glm::vec2(0, 0), glm::vec2(0, 0), 5);
	Sphere s2(glm::vec2(8, 0), glm::vec2(0, 0), 5);
	Collision col;
	Collision reverse;
	col = s1.checkSphereCollision(&s2);
	REQUIRE(col.success == true);
	REQUIRE(col.first == &s1);
	REQUIRE(col.second == &s2);
	REQUIRE(vectorApprox(col.normal, { -1,0 }));
	reverse = col.reverse();
	REQUIRE(reverse.success == true);
	REQUIRE(reverse.first == &s2);
	REQUIRE(reverse.second == &s1);
	REQUIRE(vectorApprox(reverse.normal, { 1,0 }));
}

TEST_CASE("Plane constructor", "[plane]") {
	SECTION("Distance must be finite") {
		REQUIRE_THROWS(Plane({ 1,0 }, INFINITY));
		REQUIRE_THROWS(Plane({ 1,0 }, NAN));
	}
	SECTION("Normal must be normalizable") {
		REQUIRE_THROWS(Plane({ 0,0 }, 0));
		Plane p({ 8,4 }, 0);
		REQUIRE(glm::length(p.getNormal()) == Approx(1));
	}
}

TEST_CASE("Plane setters", "[plane]") {
	Plane p({ 1,0 }, 0);

	SECTION("Distance must be finite") {
		REQUIRE_THROWS(p.setDistance(INFINITY));
		REQUIRE_THROWS(p.setDistance(NAN));
		p.setDistance(3);
		REQUIRE(p.getDistance() == 3);
	}
	SECTION("Normal must be normalizable") {
		REQUIRE_THROWS(p.setNormal({ 0,0 }));
		p.setNormal({ -7,12 });
		REQUIRE(glm::length(p.getNormal()) == Approx(1));
	}
}

TEST_CASE("Plane to point distance", "[plane], [collision]") {
	Plane p({ 1,0 }, 0);
	SECTION("Vertical plane") {
		REQUIRE(p.distanceToPoint({ 0,0 }) == Approx(0));
		REQUIRE(p.distanceToPoint({ 5,0 }) == Approx(5));
		REQUIRE(p.distanceToPoint({ -2,0 }) == Approx(-2));
		REQUIRE(p.distanceToPoint({ 0,10 }) == Approx(0));
		p.setDistance(3);
		REQUIRE(p.distanceToPoint({ 0,0 }) == Approx(3));
		REQUIRE(p.distanceToPoint({ 5,0 }) == Approx(8));
		REQUIRE(p.distanceToPoint({ -2,0 }) == Approx(1));
		REQUIRE(p.distanceToPoint({ 0,10 }) == Approx(3));
	}
	SECTION("Tilted plane") {
		p.setNormal({ 1,1 });
		REQUIRE(p.distanceToPoint({ 0,0 }) == Approx(0));
		REQUIRE(p.distanceToPoint({ 3,-3 }) == Approx(0));
		REQUIRE(p.distanceToPoint({ 7,2 }) == Approx(6.3640).margin(0.001));
		p.setDistance(-2);
		REQUIRE(p.distanceToPoint({ 0,0 }) == Approx(-2));
		REQUIRE(p.distanceToPoint({ 3,-3 }) == Approx(-2));
		REQUIRE(p.distanceToPoint({ 7,2 }) == Approx(4.3640).margin(0.001));
	}
}

TEST_CASE("Planes don't collide", "[plane], [collision]") {
	Plane p({ 1,0 }, 5);
	Plane q({ 0,1 }, 5);
	REQUIRE_FALSE(p.checkCollision(&q));
}

TEST_CASE("Plane-Sphere collision", "[plane], [collision]") {
	Plane p({ 1,0 }, -4);
	Sphere s(glm::vec2(0, 0), glm::vec2(0, 0), 5);
	Collision col;
	SECTION("vertical plane") {
		col = p.checkCollision(&s);
		if (col.first != &s) {
			col = col.reverse();
		}
		REQUIRE(col);
		REQUIRE(col.first == &s);
		REQUIRE(col.second == &p);
		REQUIRE(vectorApprox(col.normal, { 1,0 }));

		s.setPosition({ 8, 200 });
		col = p.checkCollision(&s);
		if (col.first != &s) {
			col = col.reverse();
		}
		REQUIRE(col);
		REQUIRE(col.first == &s);
		REQUIRE(col.second == &p);
		REQUIRE(vectorApprox(col.normal, { 1,0 }));

		s.setPosition({ -10, 20 });
		col = p.checkCollision(&s);
		if (col.first != &s) {
			col = col.reverse();
		}
		REQUIRE(col);
		REQUIRE(col.first == &s);
		REQUIRE(col.second == &p);
		REQUIRE(vectorApprox(col.normal, { 1,0 }));

		s.setPosition({ 10, -2 });
		col = p.checkCollision(&s);
		REQUIRE_FALSE(col);
	}
}