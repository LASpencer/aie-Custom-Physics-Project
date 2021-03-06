#include "catch.hpp"

#include "Sphere.h"
#include "Plane.h"
#include "PhysicsScene.h"

#include "Utility.h"

using namespace physics;

TEST_CASE("Sphere Constructor", "[rigidbody],[sphere]") {
	SECTION("Mass must be positive") {
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0),1, glm::vec2(0, 0), 0, -1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0),1, glm::vec2(0, 0), 0, NAN));
	}
	SECTION("Radius must be positive and finite") {
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0),0, glm::vec2(0, 0), 0,1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0),-1, glm::vec2(0, 0), 0, 1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), INFINITY, glm::vec2(0, 0), 0, 1));
		REQUIRE_THROWS(Sphere(glm::vec2(0, 0), NAN, glm::vec2(0, 0), 0, 1));
	}
	// TODO check that setting mass to 0 or infinity works correctly
}

TEST_CASE("Sphere getters and setters", "[rigidbody],[sphere]") {
	Sphere s(glm::vec2(1, 2),5, glm::vec2(3, 4), 0, 6);
	SECTION("Dynamic Sphere") {
		SECTION("Getters") {
			REQUIRE(s.getPosition() == glm::vec2(1, 2));
			REQUIRE(s.getOrientation() == 0);
			REQUIRE(s.getRadius() == 5);
			REQUIRE(s.getMass() == 6);
			REQUIRE(s.getInvMass() == Approx(1.f / 6.f));
			REQUIRE(s.isDynamic());
			REQUIRE_FALSE(s.isKinematic());
			REQUIRE_FALSE(s.isStatic());
		}
		SECTION("Setters") {
			SECTION("Mass must be positive") {
				REQUIRE_THROWS(s.setMass(-1));
				REQUIRE_THROWS(s.setMass(NAN));
			}
			s.setMass(3);
			REQUIRE(s.getMass() == 3);
			REQUIRE(s.getInvMass() == Approx(1.f / 3.f));
			SECTION("Radius must be positive and finite") {
				REQUIRE_THROWS(s.setRadius(0));
				REQUIRE_THROWS(s.setRadius(-1));
				REQUIRE_THROWS(s.setRadius(INFINITY));
				REQUIRE_THROWS(s.setRadius(NAN));
			}
		}
	}
	SECTION("Kinematic Sphere") {
		Sphere kinematic(glm::vec2(1, 2),2, glm::vec2(3, 4), 0, 0);
		Sphere kin2(glm::vec2(1, 2),2, glm::vec2(3, 4), 0, INFINITY);
		SECTION("Getters") {
			REQUIRE(kinematic.isKinematic());
			REQUIRE(kin2.isKinematic());
			REQUIRE(kinematic.getMass() == INFINITY);
			REQUIRE(kinematic.getInvMass() == 0);
			REQUIRE(kin2.getInvMass() == 0);
			REQUIRE_FALSE(kinematic.isDynamic());
			REQUIRE_FALSE(kinematic.isStatic());
			REQUIRE(kinematic.getVelocity() == glm::vec2(3, 4));
		}
		SECTION("Setters") {
			// Setting a sphere to kinematic
			s.setMass(0);
			REQUIRE(s.getMass() == INFINITY);
			REQUIRE(s.getInvMass() == 0);
			REQUIRE(s.isKinematic());
			s.setVelocity({ 2,1 });
			REQUIRE(s.getVelocity() == glm::vec2(2, 1));
		}
	}
	SECTION("Static Sphere") {
		s.setStatic(true);
		REQUIRE(s.getMass() == INFINITY);
		REQUIRE(s.getInvMass() == 0);
		REQUIRE(s.isKinematic());
		REQUIRE(s.getVelocity() == glm::vec2(0,0));
		REQUIRE(s.isStatic());
		SECTION("Setters") {
			s.setMass(3);
			REQUIRE(s.getMass() == INFINITY);
			REQUIRE(s.getInvMass() == 0);
			s.setVelocity({ 2,3 });
			REQUIRE(s.getVelocity() == glm::vec2(0, 0));
		}
	}
}

TEST_CASE("Sphere energy and momentum", "[rigidbody],[sphere],[energy],[momentum]") {
	Sphere s(glm::vec2(0,0),5, glm::vec2(3, 4), 0, 4);
	PhysicsScene* gravity = new PhysicsScene(0.1f, { 0,-10 });
	PhysicsScene* noGravity = new PhysicsScene(0.1f, { 0,0 });
	SECTION("Kinetic energy") {
		REQUIRE(s.calculateEnergy(noGravity) == Approx(50));
		s.setMass(10);
		REQUIRE(s.calculateEnergy(noGravity) == Approx(125));
		s.setVelocity({ -8,2 });
		REQUIRE(s.calculateEnergy(noGravity) == Approx(340));
	}
	SECTION("Potential energy") {
		s.setVelocity({ 0,0 });
		SECTION("Normal Gravity") {
			REQUIRE(s.calculateEnergy(gravity) == Approx(0));
			s.setPosition({ 3,5 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(200));
			s.setPosition({ 5,-2 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(-80));
			s.setVelocity({ 0, 5 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(-30));
			s.setMass(10);
			REQUIRE(s.calculateEnergy(gravity) == Approx(-75));
		}
		SECTION("Other gravity direction") {
			gravity->setGravity({ -5,7 });
			s.setPosition({ 3,5 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(-80));
			s.setPosition({ 5,-2 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(156));
			s.setVelocity({ 0, 5 });
			REQUIRE(s.calculateEnergy(gravity) == Approx(206));
			s.setMass(10);
			REQUIRE(s.calculateEnergy(gravity) == Approx(515));
		}
		
	}
	SECTION("Momentum") {
		REQUIRE(vectorApprox(s.calculateMomentum(), { 12,16 }));
		s.setMass(5);
		REQUIRE(vectorApprox(s.calculateMomentum(), { 15,20 }));
		s.setVelocity({ -2,8.5f });
		REQUIRE(vectorApprox(s.calculateMomentum(), { -10,42.5f }));
	}
	SECTION("Kinematic has no momentum or velocity") {
		s.setMass(0);
		s.setPosition({ 3,3 });
		REQUIRE(s.calculateEnergy(gravity) == 0);
		REQUIRE(s.calculateMomentum() == glm::vec2(0, 0));
	}
	SECTION("Static has no momentum or velocity") {
		s.setPosition({ 3,3 });
		s.setStatic(true);
		REQUIRE(s.calculateEnergy(gravity) == 0);
		REQUIRE(s.calculateMomentum() == glm::vec2(0, 0));
	}
	delete gravity;
	delete noGravity;
}


TEST_CASE("Sphere-sphere collision", "[rigidbody],[sphere],[collision]") {
	Sphere s1(glm::vec2(0, 0),5, glm::vec2(0, 0), 0);
	Sphere s2(glm::vec2(10, 0),5, glm::vec2(0, 0), 0);
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
		REQUIRE(collision.depth == Approx(6.f));
		REQUIRE(vectorApprox(collision.contact, { 2,0 }, k_margin));

		s2.setPosition({ 0,9 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(vectorApprox(collision.normal, {0,-1 }));
		REQUIRE(collision.depth == Approx(1.f));
		REQUIRE(vectorApprox(collision.contact, {0,4.5f }, k_margin));

		s2.setRadius(1);
		s2.setPosition({ -1,2 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(vectorApprox(collision.normal, { 0.447, -0.894 }, 0.001f));
		REQUIRE(collision.depth == Approx(3.7639f).margin(0.001f));
		REQUIRE(vectorApprox(collision.contact, { -1.3937f, 2.7875f }, 0.01f));

		s2.setPosition({ 0, 0 });
		collision = s1.checkSphereCollision(&s2);
		REQUIRE(collision.success == true);
		REQUIRE(collision.first == &s1);
		REQUIRE(collision.second == &s2);
		REQUIRE(glm::length(collision.normal) == Approx(1));
		REQUIRE(collision.depth == Approx(6.f));
		REQUIRE(collision.contact == glm::vec2(0, 0));
	}
}

TEST_CASE("Collision struct reversal", "[collision]") {
	Sphere s1(glm::vec2(0, 0),5, glm::vec2(0, 0));
	Sphere s2(glm::vec2(8, 0),5, glm::vec2(0, 0));
	Collision col;
	Collision reverse;
	col = s1.checkSphereCollision(&s2);
	REQUIRE(col.success == true);
	REQUIRE(col.first == &s1);
	REQUIRE(col.second == &s2);
	REQUIRE(vectorApprox(col.normal, { -1,0 }));
	REQUIRE(col.depth == Approx(2.f));
	REQUIRE(vectorApprox(col.contact, { 4,0 }, k_margin));
	reverse = col.reverse();
	REQUIRE(reverse.success == true);
	REQUIRE(reverse.first == &s2);
	REQUIRE(reverse.second == &s1);
	REQUIRE(vectorApprox(reverse.normal, { 1,0 }));
	REQUIRE(reverse.depth == Approx(2.f));
	REQUIRE(vectorApprox(col.contact, { 4,0 }, k_margin));
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

TEST_CASE("Planes don't have energy or momentum", "[plane],[energy],[momentum]") {
	Plane p({ 1,0 }, 0);
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,-10 });
	REQUIRE(p.calculateEnergy(scene) == 0);
	REQUIRE(p.calculateMomentum() == glm::vec2(0));
	delete scene;
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
	Sphere s(glm::vec2(0, 0),5, glm::vec2(0, 0));
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
		REQUIRE(col.depth == Approx(1));
		REQUIRE(vectorApprox(col.contact, { 3,200 }, k_margin));

		s.setPosition({ -10, 20 });
		col = p.checkCollision(&s);
		if (col.first != &s) {
			col = col.reverse();
		}
		REQUIRE(col);
		REQUIRE(col.first == &s);
		REQUIRE(col.second == &p);
		REQUIRE(vectorApprox(col.normal, { 1,0 }));
		REQUIRE(col.depth == Approx(19.f));
		REQUIRE(vectorApprox(col.contact, { -15,20 }, k_margin));

		s.setPosition({ 10, -2 });
		col = p.checkCollision(&s);
		REQUIRE_FALSE(col);
	}
}

TEST_CASE("Sphere motion", "[rigidbody],[sphere]") {
	Sphere* s = new Sphere({ 0,0 },1, { 0,0 }, 0);
	Sphere* t = new Sphere({ 0,0 },1, { 0,0 }, 0, 2.5f);
	PhysicsScene* gravity = new PhysicsScene(0.1f, { 0,-10 });
	PhysicsScene* noGravity = new PhysicsScene(0.1f, { 0,0 });
	SECTION("Sphere kinematic motion") {
		SECTION("Sphere at rest stays at rest") {
			s->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(s->getPosition(), { 0,0 }));
			REQUIRE(vectorApprox(s->getVelocity(), { 0,0 }));
		}
		SECTION("Sphere moves") {
			s->setVelocity({ 2,-1 });
			s->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(s->getPosition(), { 0.2f,-0.1f }, k_margin));
			REQUIRE(vectorApprox(s->getVelocity(), { 2,-1 }));
			for (int i = 0; i < 5; ++i) {
				s->fixedUpdate(noGravity);
			}
			REQUIRE(vectorApprox(s->getPosition(), { 1.2f,-0.6f }, k_margin));
		}
	}
	SECTION("Sphere dynamic motion") {
		SECTION("Sphere velocity changes on impulse applied") {
			s->setVelocity({ -3,6.5f });
			t->setVelocity({ -3,6.5f });
			glm::vec2 force = { 0.4f, 0.2f };
			s->applyImpulse(force);
			s->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(s->getVelocity(), { -2.6f,6.7f }, k_margin));
			REQUIRE(vectorApprox(s->getPosition(), { -0.26f, 0.67f }, k_margin));
			t->applyImpulse(force);
			t->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(t->getVelocity(), { -2.84f,6.58f }, k_margin));
			REQUIRE(vectorApprox(t->getPosition(), { -0.284f, 0.658f }, k_margin));
		}
		// HACK may have to change tests/margins based on decisions re position error vs energy error
		SECTION("Sphere velocity changes on force applied") {
			s->setVelocity({ -3,6.5f });
			t->setVelocity({ -3,6.5f });
			glm::vec2 force = { 3, -2 };
			s->applyForce(force);
			t->applyForce(force);
			s->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(s->getVelocity(), { -2.7f,6.3f }, k_margin));
			REQUIRE(vectorApprox(s->getPosition(), { -0.27f, 0.63f }, k_margin));
			t->fixedUpdate(noGravity);
			REQUIRE(vectorApprox(t->getVelocity(), { -2.88f,6.42f }, k_margin));
			REQUIRE(vectorApprox(t->getPosition(), { -0.288f, 0.642f }, k_margin));
		}
		SECTION("Gravity") {
			SECTION("Stationary") {
				s->fixedUpdate(gravity);
				t->fixedUpdate(gravity);
				REQUIRE(vectorApprox(s->getVelocity(), { 0,-1 }, k_margin));
				REQUIRE(vectorApprox(t->getVelocity(), { 0,-1 }, k_margin));
			}
			SECTION("Gravity works over time") {
				float startEnergyS = s->calculateEnergy(gravity);
				float startEnergyT = t->calculateEnergy(gravity);
				for (int i = 0; i < 50; ++i) {
					s->fixedUpdate(gravity);
					t->fixedUpdate(gravity);
				}
				glm::vec2 expectedPosition = { 0, -125 };	// 0.5 * a * t^2 = 0.5 * -10 * 5^2
				float error = 2.5001f; // timestep * time * gravity /2
				REQUIRE(vectorApprox(s->getVelocity(), { 0,-50 }, k_margin));
				REQUIRE(vectorApprox(t->getVelocity(), { 0,-50 }, k_margin));
				REQUIRE(vectorApprox(s->getPosition(), expectedPosition, error));
				REQUIRE(vectorApprox(t->getPosition(), expectedPosition, error));
				float energyError = 25.0001f; // Height error * gravity
				REQUIRE(s->calculateEnergy(gravity) == Approx(startEnergyS).margin(energyError * s->getMass()));
				REQUIRE(t->calculateEnergy(gravity) == Approx(startEnergyT).margin(energyError * t->getMass()));
			}
			SECTION("Ballistic arc") {
				s->setVelocity({ 8,20 });
				t->setVelocity({ 8,20 });
				float startEnergyS = s->calculateEnergy(gravity);
				float startEnergyT = t->calculateEnergy(gravity);
				for (int i = 0; i < 50; ++i) {
					s->fixedUpdate(gravity);
					t->fixedUpdate(gravity);
				}
				glm::vec2 expectedPosition = { 40, -25 };
				float error = 2.5001f; // timestep * time * gravity /2
				REQUIRE(vectorApprox(s->getVelocity(), { 8,-30 }, k_margin));
				REQUIRE(vectorApprox(t->getVelocity(), { 8,-30 }, k_margin));
				REQUIRE(vectorApprox(s->getPosition(), expectedPosition, error));
				REQUIRE(vectorApprox(t->getPosition(), expectedPosition, error));
				float energyError = 25.0001f; // Height error * gravity
				REQUIRE(s->calculateEnergy(gravity) == Approx(startEnergyS).margin(energyError * s->getMass()));
				REQUIRE(t->calculateEnergy(gravity) == Approx(startEnergyT).margin(energyError * t->getMass()));
			}
		}
		SECTION("Reaction forces") {
			SECTION("Stationary") {
				glm::vec2 force = {8,-3};
				glm::vec2 startMomentum = s->calculateMomentum() + t->calculateMomentum();
				SECTION("Impulse") {
					s->applyImpulseFromOther(t, force);
					REQUIRE(vectorApprox(s->getVelocity(), { 8,-3 }, k_margin));
					REQUIRE(vectorApprox(t->getVelocity(), { -3.2f,1.2f }, k_margin));
					REQUIRE(vectorApprox(s->calculateMomentum() + t->calculateMomentum(), startMomentum, k_margin));
				}
				SECTION("Force") {
					s->applyForceFromOther(t, force);
					s->fixedUpdate(noGravity);
					t->fixedUpdate(noGravity);
					REQUIRE(vectorApprox(s->getVelocity(), { 0.8f,-0.3f }, k_margin));
					REQUIRE(vectorApprox(t->getVelocity(), { -0.32f,0.12f }, k_margin));
					REQUIRE(vectorApprox(s->calculateMomentum() + t->calculateMomentum(), startMomentum, k_margin));
				}
			}
		}
	}
	delete s;
	delete t;
	delete gravity;
	delete noGravity;
}

//TODO set up and resolve collisions between dynamic and kinematic/static
TEST_CASE("Forces don't affect kinematic and static spheres", "[rigidbody],[sphere]") {
	Sphere* s = new Sphere({ 0,0 },1, { 0,0 });
	Sphere* t = new Sphere({ 0,0 },1, { 0,0 }, 0, 0);
	Sphere* u = new Sphere({ 0,0 },1, { 0,0 }, 0, 0);
	u->setStatic(true);

	glm::vec2 force = { 8,-3 };

	t->applyImpulse(force);
	REQUIRE(t->getVelocity() == glm::vec2(0, 0));
	t->applyImpulseFromOther(s, force);
	REQUIRE(t->getVelocity() == glm::vec2(0, 0));
	REQUIRE(vectorApprox(s->getVelocity(), { -8,3 },k_margin));
	u->applyImpulse(force);
	REQUIRE(u->getVelocity() == glm::vec2(0, 0));
	delete s;
	delete t;
	delete u;
}

TEST_CASE("Rigidbody rotates", "[sphere],[rotation]") {
	Sphere* s = new Sphere({ 0,0 },1, { 0,0 });
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,0 });
	s->setAngularVelocity(1);
	s->fixedUpdate(scene);
	REQUIRE(s->getAngularVelocity() == 1.f);
	REQUIRE(s->getOrientation() == Approx(0.1f));
	delete s;
	delete scene;
}

TEST_CASE("Sphere moment of inertia", "[sphere],[rotation]") {
	Sphere* s = new Sphere({ 0,0 },1, { 0,0 }, 0, 1);
	REQUIRE(s->getMoment() == Approx(0.5f));
	REQUIRE(s->getInvMoment() == Approx(2));
	s->setMass(3);
	REQUIRE(s->getMoment() == Approx(1.5f));
	REQUIRE(s->getInvMoment() == Approx(0.66667f).margin(0.0001f));
	s->setRadius(2.5f);
	REQUIRE(s->getMoment() == Approx(9.375f));
	REQUIRE(s->getInvMoment() == Approx(0.10667f).margin(0.0001f));
	
	SECTION("Kinematic has infinite moment") {
		s->setMass(0);
		REQUIRE(s->getMoment() == INFINITY);
		REQUIRE(s->getInvMoment() == 0);
	}
	SECTION("Static has infinite moment") {
		s->setStatic(true);
		REQUIRE(s->getMoment() == INFINITY);
		REQUIRE(s->getInvMoment() == 0);
	}
	delete s;
}

TEST_CASE("Applying torque to sphere", "[sphere],[rotation]") {
	Sphere* s = new Sphere({ 0,0 }, 4, { 0,0 }, 0, 3);
	PhysicsScene* scene = new PhysicsScene(0.1f, { 0,0 });
	REQUIRE(s->getMoment() == Approx(24));
	SECTION("Applying impulse") {
		s->applyImpulse({ 0,2 }, { 3,0 });
		REQUIRE(s->getAngularVelocity() == Approx(0.25f));
		s->applyImpulse({ 3,0 }, {1,1});
		REQUIRE(s->getAngularVelocity() == Approx(0.125f));
	}
	SECTION("Applying force") {
		s->applyForce({ 3,2 }, { 3,1 });
		s->fixedUpdate(scene);
		REQUIRE(s->getAngularVelocity() == Approx(0.0125f));
	}
	delete s;
	delete scene;
}