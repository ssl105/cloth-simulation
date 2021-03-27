////////////////////////////////////////
// Ground.cpp
////////////////////////////////////////

#include "Ground.h"

////////////////////////////////////////////////////////////////////////////////

Ground::Ground(int size, float min) {
	elevation = min;

	// Model matrix.
	model = glm::mat4(1.0f);

	// The color of the ground.
	color = glm::vec3(0.2f, 0.55f, 0.8f);

	// normal
	normal = glm::vec3(0, 1, 0);


	positions.push_back(glm::vec3(-size, elevation, -size));
	positions.push_back(glm::vec3(-size, elevation, size));
	positions.push_back(glm::vec3(size, elevation, size));
	positions.push_back(glm::vec3(size, elevation, -size));

	for (int i = 0; i < 4; i++) {
		normals.push_back(normal);
	}
	
	
	// Each ivec3(v1, v2, v3) define a triangle consists of vertices v1, v2
	// and v3 in counter-clockwise order.
	indices.push_back(glm::ivec3(0, 1, 2));
	indices.push_back(glm::ivec3(0, 2, 3));

	
	// Generate a vertex array (VAO) and vertex buffer object (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

////////////////////////////////////////////////////////////////////////////////

Ground::~Ground() {


	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

////////////////////////////////////////////////////////////////////////////////

void Ground::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	// actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, sizeof(glm::ivec3) * indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

float Ground::getElevation() {
	return elevation;
}

////////////////////////////////////////////////////////////////////////////////

glm::vec3 Ground::getNormal() {
	return normal;
}
