#ifndef _SPRINGDAMPER_H_
#define _SPRINGDAMPER_H_

#include "core.h"
#include "Particle.h"

////////////////////////////////////////////////////////////////////////////////

class SpringDamper
{
private:
	float SpringConstant, DampingConstant, RestLength;
	Particle *P1, *P2;


public:
	SpringDamper(Particle * p1, Particle* p2, float ks, float kd, float l0);

	void computeForce();
};

////////////////////////////////////////////////////////////////////////////////

#endif
