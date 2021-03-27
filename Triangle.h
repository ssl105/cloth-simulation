#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "core.h"
#include "Particle.h"

////////////////////////////////////////////////////////////////////////////////

class Triangle
{
private:
	// Triangle information
	glm::vec3 normal;

	Particle* v1, * v2, * v3;

public:
	Triangle(Particle *p1, Particle* p2, Particle * p3);
	
	void calculateNormal();
	void applyNormal();
	glm::vec3 calculateVelocity();
	void calculateDragForce(glm::vec3 windSpeed, float density, float dragCoeff);

	float getArea();

};

////////////////////////////////////////////////////////////////////////////////

#endif
