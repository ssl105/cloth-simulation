////////////////////////////////////////
// Triangle.cpp
////////////////////////////////////////

#include "Triangle.h"

////////////////////////////////////////////////////////////////////////////////

Triangle::Triangle(Particle* p1, Particle* p2, Particle* p3) {
	v1 = p1;
	v2 = p2;
	v3 = p3;

	calculateNormal();
}

////////////////////////////////////////////////////////////////////////////////

void Triangle::calculateNormal() {
	normal = glm::cross(v2->getPosition() - v1->getPosition(), v3->getPosition() - v1->getPosition());
	normal = glm::normalize(normal);

	applyNormal();

}

////////////////////////////////////////////////////////////////////////////////

void Triangle::applyNormal() {
	v1->addNormal(normal);
	v2->addNormal(normal);
	v3->addNormal(normal);

}

////////////////////////////////////////////////////////////////////////////////

glm::vec3 Triangle::calculateVelocity() {
	return (v1->getVelocity() + v2->getVelocity() + v3->getVelocity())/3.0f;

}

////////////////////////////////////////////////////////////////////////////////

void Triangle::calculateDragForce(glm::vec3 windSpeed, float density, float dragCoeff) {
	glm::vec3 v = calculateVelocity() - windSpeed;
	float vlength = glm::length(v);
	if (vlength == 0) {
		return;
	}
	float a = getArea() * (glm::dot(glm::normalize(v), normal));
	float lsquare = vlength * vlength;

	glm::vec3 aforce = (-0.5f) * density * lsquare * dragCoeff * a * normal;

	v1->applyForce(aforce);
	v2->applyForce(aforce);
	v3->applyForce(aforce);
}


////////////////////////////////////////////////////////////////////////////////

float Triangle::getArea() {
	return glm::length(glm::cross(v2->getPosition() - v1->getPosition(), v3->getPosition() - v1->getPosition()))/2;
}