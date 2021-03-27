////////////////////////////////////////
// Cloth.cpp
////////////////////////////////////////

#include "Cloth.h"

////////////////////////////////////////////////////////////////////////////////

Cloth::Cloth(int length, float springConstant, float damperConstant, float dragCoeff, float rest, float fric) {
	srand((unsigned)time(NULL)); // rng

	// Model matrix.
	model = glm::mat4(1.0f);

	// The color of the cloth.
	color = glm::vec3(1.0f, 0.95f, 0.9f);

	// set constants
	dc = dragCoeff;
	restitution = rest;
	friction = fric;

	//intialize particles
	float startPos = - length / 2.0;
	int numParticles = length * ppu + 1;	// num particles in each row
	
	for (int i = 0; i < numParticles; i++) {
		for (int j = 0; j < numParticles; j++) {
			bool fixed = false;
			glm::vec3 pos;
			pos.x = startPos + j * rLength;
			pos.y = -startPos - i * rLength;
			if (i == -1) {
				pos.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.1f) - 0.05f;
			}
			else {
				pos.z = 0;
			}
			

			if ( i==0) {
				fixed = true;
			}

			particles.push_back(new Particle(pos, mass, fixed));
			positions.push_back(pos);
		}
	}

	// initialize springDampers
	for (int i = 0; i < particles.size(); i++) {
		int row = i / numParticles;
		int col = i % numParticles;

		if (row == numParticles - 1) {
			if (col != numParticles - 1) {
				springDampers.push_back(new SpringDamper(particles[i], particles[i + 1], springConstant, damperConstant, rLength));
			}
		}
		else {
			if (col == 0) {
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles], springConstant, damperConstant, rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles + 1], springConstant, damperConstant, sqrt(2)*rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + 1], springConstant, damperConstant, rLength));
			}
			else if (col == numParticles -1) {
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles], springConstant, damperConstant, rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles - 1], springConstant, damperConstant, sqrt(2)*rLength));
			}
			else {
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles - 1], springConstant, damperConstant, sqrt(2)*rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles], springConstant, damperConstant, rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + numParticles + 1], springConstant, damperConstant, sqrt(2)*rLength));
				springDampers.push_back(new SpringDamper(particles[i], particles[i + 1], springConstant, damperConstant, rLength));
			}
		}

	}

	// intialize triangles and indices
	for (int i = 0; i < particles.size() - numParticles; i++) {
		int col = i % numParticles;
		if (col != numParticles - 1) {
			Triangle* curr;
			glm::vec3 ind1, ind2;
			ind1.x = i;
			ind1.y = i + numParticles;
			ind1.z = i + numParticles + 1;

			ind2.x = i;
			ind2.y = i + numParticles + 1;
			ind2.z = i + 1;

			triangles.push_back(curr = new Triangle(particles[ind1.x], particles[ind1.y], particles[ind1.z]));
			triangles.push_back(curr = new Triangle(particles[ind2.x], particles[ind2.y], particles[ind2.z]));

			indices.push_back(ind1);
			indices.push_back(ind2);;
		}
	}

	// intialize normals
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->setNormal(glm::normalize(particles[i]->getNormal()));

		normals.push_back(particles[i]->getNormal());
	}

	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////

Cloth::~Cloth() {
	// delete cloth info
	for (int i = 0; i < particles.size(); i++) {
		delete particles[i];
	}

	for (int i = 0; i < springDampers.size(); i++) {
		delete springDampers[i];
	}

	for (int i = 0; i < triangles.size(); i++) {
		delete triangles[i];
	}

	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

////////////////////////////////////////////////////////////////////////////////

void Cloth::draw(const glm::mat4& viewProjMtx, GLuint shader)
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
	glDrawElements(GL_TRIANGLES, sizeof(glm::ivec3)* indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

void Cloth::update(float deltaTime, glm::vec3 gravity, glm::vec3 windSpeed, float density)
{
	//apply gravity to particles
	gravity = mass * gravity;

	for (int i = 0; i < particles.size(); i++) {
		particles[i]->applyForce(gravity);
	}

	//apply spring damper forces
	for (int i = 0; i < springDampers.size(); i++) {
		springDampers[i]->computeForce();
	}

	// apply aerodynamics
	for (int i = 0; i < triangles.size(); i++) {
		triangles[i]->calculateDragForce(windSpeed, density, dc);
	}

	// integrate motion
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->integrate(deltaTime);
	}

	updateNormAndPos();

}

////////////////////////////////////////////////////////////////////////////////

void Cloth::updateNormAndPos() {
	//calculate normals
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->setNormal(glm::vec3(0));
	}

	for (int i = 0; i < triangles.size(); i++) {
		triangles[i]->calculateNormal();
	}


	// update normals and positions
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->setNormal(glm::normalize(particles[i]->getNormal()));

		normals[i] = particles[i]->getNormal();
		positions[i] = particles[i]->getPosition();
	}
}

////////////////////////////////////////////////////////////////////////////////

void Cloth::updateBuffer() {

	// update buffers and send to shader
	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * positions.size(), positions.data());
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * normals.size(), normals.data());
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

////////////////////////////////////////////////////////////////////////////////

void Cloth::checkGroundCollision(Ground* ground) {
	float elevation = ground->getElevation();
	glm::vec3 gNorm = ground->getNormal();
	bool collision = false;

	for (int i = 0; i < particles.size(); i++) {
		glm::vec3 pos = particles[i]->getPosition();

		if (pos.y < elevation) {
			if (!collision) {
				collision = true;
			}

			// handle collison
			glm::vec3 v = particles[i]->getVelocity();
			glm::vec3 vNorm = glm::dot(v, gNorm)*gNorm;
			glm::vec3 vTan = v - vNorm;
			glm::vec3 impulse = -(1 + restitution) * mass * vNorm;
			glm::vec3 fimpulse = friction * glm::length(impulse) * -vTan;
			impulse = impulse + fimpulse;
			glm::vec3 deltav = impulse / mass;

			particles[i]->addVelocity(deltav);	// apply impulse to velociy

			pos.y = elevation + 0.001;

			particles[i]->setPosition(pos);	// push particle above levation
		}
	}

	if (collision) {
		updateNormAndPos();
		
	}
}

////////////////////////////////////////////////////////////////////////////////

void Cloth::translate(glm::vec3 dir) {
	for (int i = 0; i < particles.size(); i++) {
		if (particles[i]->isFixed()) {
			glm::vec3 pos = particles[i]->getPosition();

			pos += dir;

			particles[i]->setPosition(pos);
		}
	}

	updateNormAndPos();
	updateBuffer();
}