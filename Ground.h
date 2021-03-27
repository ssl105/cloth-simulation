#ifndef _GROUND_H_
#define _GROUND_H_

#include "core.h"

////////////////////////////////////////////////////////////////////////////////

class Ground
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO;

	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::ivec3> indices;

	float elevation;
	float frictionForce;

	glm::mat4 model;
	glm::vec3 color;
	glm::vec3 normal;

public:
	Ground(int size, float elevation);
	~Ground();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);

	float getElevation();
	glm::vec3 getNormal();
};

////////////////////////////////////////////////////////////////////////////////

#endif
