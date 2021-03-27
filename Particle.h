#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "core.h"

////////////////////////////////////////////////////////////////////////////////

class Particle
{
private:
	glm::vec3 position, normal, velocity, force;
	float mass;

	bool fixed;

public:
	Particle(glm::vec3 pos, float m, bool f);

	void applyForce(glm::vec3 f);
	void integrate(float deltaTime);

	glm::vec3 getPosition();
	glm::vec3 getVelocity();
	glm::vec3 getNormal();
	bool isFixed();

	void setNormal(glm::vec3 norm);
	void setPosition(glm::vec3 pos);

	void addNormal(glm::vec3 norm);
	void addVelocity(glm::vec3 v);

};

////////////////////////////////////////////////////////////////////////////////

#endif
