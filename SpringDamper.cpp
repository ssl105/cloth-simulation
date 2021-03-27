////////////////////////////////////////
// SpringDamper.cpp
////////////////////////////////////////

#include "SpringDamper.h"

////////////////////////////////////////////////////////////////////////////////

SpringDamper::SpringDamper(Particle* p1, Particle* p2, float ks, float kd, float l0) {
	SpringConstant = ks;
	DampingConstant = kd;
	RestLength = l0;

	P1 = p1;
	P2 = p2;

}

////////////////////////////////////////////////////////////////////////////////

void SpringDamper::computeForce() {
	// get current length
	glm::vec3 e = P2->getPosition() - P1->getPosition();
	float l = glm::length(e);
	e = glm::normalize(e);

	// get closing velocity
	float vClose = glm::dot(P1->getVelocity() - P2->getVelocity(), e);

	// compute final forces
	glm::vec3 f1, f2;
	float f = -SpringConstant * (RestLength - l) - DampingConstant * vClose;

	f1 = f * e;
	f2 = -f1;

	// apply forces
	P1->applyForce(f1);
	P2->applyForce(f2);

}

////////////////////////////////////////////////////////////////////////////////
