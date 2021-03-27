#ifndef _CLOTH_H_
#define _CLOTH_H_

#include "core.h"
#include "Particle.h"
#include "SpringDamper.h"
#include "Triangle.h"
#include "Ground.h"

#include <time.h>
#include <math.h>
#include <limits>

////////////////////////////////////////////////////////////////////////////////

class Cloth
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::vec3 color;

	// Cloth Info
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::ivec3> indices;
	vector<Particle*> particles;
	vector<SpringDamper*> springDampers;
	vector<Triangle*> triangles;

	float dc;	// damper constant
	float restitution;	// for collision
	float friction;	// friction coefficient

	// constant
	const int ppu = 5; // particles per unit
	const float rLength = 1.0 / ppu;  // distance between two particles
	const float mass = 0.1;


public:
	Cloth(int length, float springConstant, float damperConstant, float dragCoeff, float rest, float frict);
	~Cloth();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update(float deltaTime, glm::vec3 gravity, glm::vec3 windSpeed, float density);
	void updateNormAndPos();
	void updateBuffer();
	void checkGroundCollision(Ground* ground);
	void translate(glm::vec3 dir);

};

////////////////////////////////////////////////////////////////////////////////

#endif
