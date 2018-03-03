#include "Box.h"

#include "Plane.h"
#include "Sphere.h"

physics::Box::Box(glm::vec2 position, float width, float height, float orientation, glm::vec2 velocity,
	float angularVelocity, float mass, float elasticity, float friction, float drag, float angularDrag, glm::vec4 colour) :
	RigidBody(position,velocity,orientation,mass,elasticity,angularVelocity,friction,drag,angularDrag,colour), m_xExtent(0.5f * width), m_yExtent(0.5f * height)
{
	if (m_xExtent <= 0 || isnan(m_xExtent) || isinf(m_xExtent)) {
		throw std::invalid_argument("Width must be positive and finite");
	}
	if (m_yExtent <= 0 || isnan(m_yExtent) || isinf(m_yExtent)) {
		throw std::invalid_argument("Height must be positive and finite");
	}
	calculateMoment();
}

physics::Box::Box(const Box & other) :
	RigidBody(other), m_xExtent(other.m_xExtent), m_yExtent(other.m_yExtent)
{
}

physics::PhysicsObject * physics::Box::clone()
{
	return new Box(*this);
}

void physics::Box::makeGizmo(float timeRatio)
{
	glm::vec2 interpPos = glm::mix(m_pastPosition, m_position, timeRatio);
	// HACK not normalizing to save time, might need to do so if it looks bad/based on speed
	glm::vec2 interX = glm::mix(m_pastX, m_localX , timeRatio) * m_xExtent;
	glm::vec2 interY = glm::mix(m_pastY, m_localY, timeRatio) * m_yExtent;

	glm::vec2 p1 = interpPos - interX - interY;
	glm::vec2 p2 = interpPos + interX - interY;
	glm::vec2 p3 = interpPos + interX + interY;
	glm::vec2 p4 = interpPos - interX + interY;

	aie::Gizmos::add2DTri(p1, p2, p3, m_colour);
	aie::Gizmos::add2DTri(p1, p3, p4, m_colour);
}

bool physics::Box::isPointInside(glm::vec2 point)
{
	glm::vec2 localPoint = worldToLocalSpace(point);
	return (abs(localPoint.x) < m_xExtent && abs(localPoint.y) < m_yExtent);
}

physics::Collision physics::Box::checkCollision(PhysicsObject * other)
{
	return other->checkBoxCollision(this);
}

physics::Collision physics::Box::checkSphereCollision(Sphere * other)
{
	Collision collision(true, this, other);
	// Start by assuming collision
	glm::vec2 displacement = m_position - other->getPosition();
	if (displacement != glm::zero<glm::vec2>()) {
		glm::vec2 minAxis;
		float minOverlap = INFINITY;
		std::array<glm::vec2, 4> corners = getCorners();
		// Get axes to test: x, y, and circle to nearest corner

		float xProjection = copysignf(m_xExtent, glm::dot(displacement, m_localX));
		float yProjection = copysignf(m_yExtent, glm::dot(displacement, m_localY));
		glm::vec2 minCircleToCorner = displacement - m_localX * xProjection - m_localY * yProjection;
		//float minCornerDistanceSqr = INFINITY;
		//for (glm::vec2 corner : corners) {
		//	glm::vec2 circleToCorner = corner - other->getPosition();
		//	float cornerDistanceSqr = glm::dot(circleToCorner, circleToCorner);
		//	if (cornerDistanceSqr < minCornerDistanceSqr) {
		//		minCornerDistanceSqr = cornerDistanceSqr;
		//		minCircleToCorner = circleToCorner;
		//	}
		//}

		if (minCircleToCorner == glm::zero<glm::vec2>()) {
			// If corner in circle, axis is to center
			minCircleToCorner = displacement;
		}
		glm::vec2 axes[3] = { m_localX, m_localY, glm::normalize(minCircleToCorner) };
		// Test circle/box overlap on each axis
		for(size_t a = 0; a < 3; ++a ) {
			float boxMin = INFINITY;
			float boxMax = -INFINITY;
			for(size_t i = 0; i < 4; ++i ) {
				float projection = glm::dot(corners[i], axes[a]);
				boxMin = std::min(boxMin, projection);
				boxMax = std::max(boxMax, projection);
			}
			float circleProjection = glm::dot(other->getPosition(), axes[a]);
			float circleMax = circleProjection + other->getRadius();
			float circleMin = circleProjection - other->getRadius();
			float overlap = std::min(boxMax - circleMin, circleMax - boxMin);
			if (overlap <= 0) {
				// Axis separation found, so stop checking
				collision.success = false;
				break;
			}
			else if (overlap < minOverlap) {
				// if new smallest overlap, set axis and depth
				minOverlap = overlap;
				// If axis points to circle, reverse its direction
				if (boxMax - circleMin < circleMax - boxMin) {
					minAxis = -axes[a];
				}
				else {
					minAxis = axes[a];
				}

			}
		}
		if (collision.success) {
			collision.normal = minAxis;
			collision.depth = minOverlap;
			collision.contact = other->getPosition() + minAxis * (other->getRadius() - 0.5f * minOverlap);
		}
	}
	else {
		// Circle is at position
		// Collision normal is towards shortest axis
		if (m_xExtent < m_yExtent) {
			collision.normal = m_localX;
			collision.contact = m_position;
			collision.depth = m_xExtent + other->getRadius();
		}
		else {
			collision.normal = m_localY;
			collision.contact = m_position;
			collision.depth = m_yExtent + other->getRadius();
		}
	}

	return collision;
}

physics::Collision physics::Box::checkBoxCollision(Box * other)
{
	// Start by assuming collision
	Collision collision(true, this, other);
	std::array<glm::vec2, 4> myCorners = getCorners();
	std::array<glm::vec2, 4> otherCorners = other->getCorners();
	// Get axes to test
	glm::vec2 axes[4] = { m_localX, m_localY, other->getLocalX(), other->getLocalY() };
	glm::vec2 minAxis;
	float minOverlap = INFINITY;
	float myDistance, otherDistance; // Distance of points from overlap along axis
	for (size_t a = 0; a < 4; ++a) {
		// Get min and max projections along axis
		float myMin = INFINITY;
		float otherMin = INFINITY;
		float myMax = -INFINITY;
		float otherMax = -INFINITY;
		for (size_t i = 0; i < 4; ++i) {
			float myProjection = glm::dot(axes[a], myCorners[i]);
			float otherProjection = glm::dot(axes[a], otherCorners[i]);
			myMin = std::min(myMin, myProjection);
			myMax = std::max(myMax, myProjection);
			otherMin = std::min(otherMin, otherProjection);
			otherMax = std::max(otherMax, otherProjection);
		}
		// Check overlap
		float overlap = std::min(myMax - otherMin, otherMax - myMin);
		if (overlap <= 0) {
			// If negative, collision fails
			collision.success = false;
			break;
		}
		else if (overlap < minOverlap) {
			// If new minimum, set as axis of collision
			minOverlap = overlap;
			if (myMax - otherMin < otherMax - myMin) {
				minAxis = -axes[a];
				myDistance = -myMax;
				otherDistance = -otherMin;
			}
			else {
				minAxis = axes[a];
				myDistance = myMin;
				otherDistance = otherMax;
			}
		}
	}
	if (collision.success) {
		collision.normal = minAxis;
		collision.depth = minOverlap;
		// Find best edges involved in collision
		Edge myEdge = findBestEdge(-minAxis);
		Edge otherEdge = other->findBestEdge(minAxis);
		// Clip each edge's endpoints to within the other
		otherEdge.clip(myEdge.direction, glm::dot(myEdge.direction, myEdge.start));
		otherEdge.clip(-myEdge.direction, glm::dot(-myEdge.direction, myEdge.end));
		myEdge.clip(otherEdge.direction, glm::dot(otherEdge.direction, otherEdge.start));
		myEdge.clip(-otherEdge.direction, glm::dot(-otherEdge.direction, otherEdge.end));
		//  check for intersection to determine if triangle or quad manifold
		glm::vec2 intersection;
		bool linesIntersect = myEdge.checkIntersection(otherEdge, intersection);
		if (linesIntersect) {
			// Figure out which other two points define manifold
			float otherAlignment = glm::dot(otherEdge.direction, collision.normal);
			float myAlignment = glm::dot(myEdge.direction, collision.normal);
			glm::vec2 myPoint, otherPoint;
			// Check which edge is less perpendicular to collision normal
			if (abs(otherAlignment) > abs(myAlignment)) {
				if (otherAlignment > 0) {
					// End of other is closest to me
					otherPoint = otherEdge.end;
					myPoint = myEdge.start;	// Known through winding
				}
				else {
					// Start of other is closest to me
					otherPoint = otherEdge.start;
					myPoint = myEdge.end; // Known through winding
				}
			}
			else {
				// Since normal points to this, opposite of above case
				if (myAlignment > 0) {
					// My start is further behind edge
					myPoint = myEdge.start;
					otherPoint = otherEdge.end;
				}
				else {
					// My end is further behind edge
					myPoint = myEdge.end;
					otherPoint = otherEdge.start;
				}
			}
			// Triangle centroid is average
			collision.contact = (myPoint + otherPoint + intersection) * (1.f / 3.f);
		}
		else {
			// Quadrilateral manifold
			// HACK maybe try centroid calculation?
			// Average 
			collision.contact = 0.25f * (myEdge.start + myEdge.end + otherEdge.start + otherEdge.end);
		}
		
	}
	return collision;
}

void physics::Box::calculateMoment()
{
	// based on 1/12 * m * width * height. Some sources say 1/12 * mass * (width^2 + height^2)
	m_moment = (4.f / 12.f) * m_mass * m_xExtent * m_yExtent;	// 4/12 because extents are half total
	if (m_moment == INFINITY) {
		m_invMoment = 0;
	}
	else {
		m_invMoment = 1.f / m_moment;
	}
}

physics::Collision physics::Box::checkPlaneCollision(Plane * other)
{
	Collision collision(false, this, other);
	std::array<glm::vec2, 4> corners = getCorners();
	float minDistance = INFINITY;
	float sumDepth = 0;
	glm::vec2 contact = { 0,0 };
	// Check each corner's distance to plane, and get lowest
	for (size_t i = 0; i < 4; ++i) {
		float distance = other->distanceToPoint(corners[i]);
		if (distance < minDistance) {
			minDistance = distance;
		}
		if (distance < 0) {
			// If colliding, add to contact for weighted average
			collision.success = true;
			sumDepth += distance;
			contact += corners[i] * distance;
		}
	}
	if (collision.success) {
		collision.depth = -minDistance;
		collision.normal = other->getNormal();
		collision.contact = contact / sumDepth;
	}
	return collision;
}

physics::ShapeType physics::Box::getShapeID()
{
	return ShapeType::obox;
}

float physics::Box::getWidth()
{
	return 2 * m_xExtent;
}

void physics::Box::setWidth(float width)
{
	if (width <= 0 || isnan(width) || isinf(width)) {
		throw std::invalid_argument("Width must be positive and finite");
	}
	m_xExtent = 0.5f * width;
	calculateMoment();
}

float physics::Box::getHeight()
{
	return 2 * m_yExtent;
}

void physics::Box::setHeight(float height)
{
	if (height <= 0 || isnan(height) || isinf(height)) {
		throw std::invalid_argument("Height must be positive and finite");
	}
	m_yExtent = 0.5f * height;
	calculateMoment();
}

float physics::Box::getDiagonalLength()
{
	return 2 * sqrtf(m_xExtent * m_xExtent + m_yExtent * m_yExtent);
}

glm::vec2 physics::Box::getXExtent()
{
	return m_localX * m_xExtent;
}

glm::vec2 physics::Box::getYExtent()
{
	return m_localY * m_yExtent;
}

std::array<glm::vec2, 4> physics::Box::getCorners()
{
	glm::vec2 x = m_xExtent * m_localX;
	glm::vec2 y = m_yExtent * m_localY;
	return { m_position - x - y, m_position + x - y,
			 m_position + x + y, m_position - x + y };
}

physics::Edge physics::Box::findBestEdge(glm::vec2 normal)
{
	Edge edge;
	glm::vec2 first, second, direction;
	bool positive;
	float x = glm::dot(normal, m_localX);
	float y = glm::dot(normal, m_localY);
	
	// Figure out if best edge is on x or y axis
	if (abs(x) > abs(y)) {
		first = m_localX * m_xExtent - m_localY * m_yExtent;
		second = m_localX * m_xExtent + m_localY * m_yExtent;
		direction = m_localY;
		positive = x > 0;
	}
	else {
		first = m_localY * m_yExtent + m_localX * m_xExtent;
		second = m_localY * m_yExtent - m_localX * m_xExtent;
		direction = -m_localX;
		positive = y > 0;
	}

	// If on negative side, reverse vectors
	if (positive) {
		edge.start = m_position + first;
		edge.end = m_position + second;
		edge.direction = direction;
	}
	else {
		edge.start = m_position - first;
		edge.end = m_position - second;
		edge.direction = -direction;
	}
	return edge;
}

void physics::Edge::clip(glm::vec2 normal, float offset)
{
	float startDistance = glm::dot(start, normal) - offset;
	float endDistance = glm::dot(end, normal) - offset;
	if (startDistance * endDistance < 0) {
		// Proportion of edge at which zero is found
		float zeroPoint = startDistance / (startDistance - endDistance);
		glm::vec2 endpoint = start + zeroPoint * (end - start);
		if (startDistance < 0) {
			start = endpoint;
		}
		else {
			end = endpoint;
		}
	}
}

bool physics::Edge::checkIntersection(const Edge & other, glm::vec2 & intersection) const
{
	float det = (direction.y * other.direction.x) - (direction.x * other.direction.y);
	if (det != 0) {
		float detFactor = 1.f / det;
		glm::vec2 displacement = other.start - start;
		float myDistance = detFactor * glm::dot({ -other.direction.y, other.direction.x }, displacement);
		float otherDistance = detFactor * glm::dot({ -direction.y, direction.x }, displacement);
		if (myDistance >= 0 && otherDistance >= 0 && myDistance <= getLength() && otherDistance <= other.getLength()) {
			intersection = start + myDistance * direction;
			return true;
		}
	}
	return false;
}
