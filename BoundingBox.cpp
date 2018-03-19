#include "BoundingBox.h"
#include "Window.h"
BoundingBox::BoundingBox(std::vector<glm::vec3> v, glm::vec3 move) {
	minX = 10000000.0f;
	maxX = -10000000.0f;
	minY = 10000000.0f;
	maxY = -10000000.0f;
	minZ = 10000000.0f;
	maxZ = -10000000.0f;
	color = { 0.0f,0.0f,0.0f };
	for (int i = 0;i < v.size();i++) {
		if (v[i].x > maxX) {
			maxX = v[i].x;
		}
		if (v[i].x < minX) {
			minX = v[i].x;
		}

		if (v[i].y > maxY) {
			maxY = v[i].y;
		}

		if (v[i].y < minY) {
			minY = v[i].y;
		}

		if (v[i].z > maxZ) {
			maxZ = v[i].z;
		}
		if (v[i].z < minZ) {
			minZ = v[i].z;
		}
	}
	offset = move;
	toWorld = glm::translate(glm::mat4(1.0f), move);
	/*minX += move.x;
	maxX += move.x;
	minY += move.y;
	maxY += move.y;
	minZ += move.z;
	maxZ += move.z;*/

	float vertices[8][3] = {
		{ maxX, maxY, maxZ }, //0
	{ maxX, maxY, minZ }, //1
	{ maxX, minY, minZ }, //2
	{ minX, minY, minZ }, //3
	{ minX, minY, maxZ }, //4
	{ minX, maxY, maxZ }, //5
	{ minX, maxY, minZ }, //6
	{ maxX, minY, maxZ }  //7
	};

	int indices[12][2] = {
		{ 5,4 },
	{ 5,0 },

	{ 4,7 },
	{ 7,0 },

	{ 6,5 },
	{ 1,0 },

	{ 3,4 },
	{ 2,7 },

	{ 6,3 },
	{ 6,1 },

	{ 3,2 },
	{ 2,1 }
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

					 // We've sent the vertex data over to OpenGL, but there's still something missing.
					 // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);
}


BoundingBox::~BoundingBox() {

}

void BoundingBox::draw(GLuint shaderProgram) {
	//glm::vec3 size = glm::vec3(maxX - minX, maxY - minY, maxZ - minZ);
	//glm::vec3 center = glm::vec3((minX + maxX) / 2, (minY + maxY) / 2, (minZ + maxZ) / 2);
	//glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	///* Apply object's transformation matrix */
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
	//glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glm::mat4 modelview = Window::V * toWorld;
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);

	glUniform3f(glGetUniformLocation(shaderProgram, "colorInput"), color.x, color.y, color.z);
	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
	glDrawElements(GL_LINE_STRIP, 24, GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void BoundingBox::update(glm::vec3 move) {
	offset += move;
	toWorld = glm::translate(glm::mat4(1.0f), offset);
}

int BoundingBox::detectCollision(BoundingBox* other) {
	if ((minX+ offset.x) > (other->maxX+other->offset.x) || (maxX+offset.x) < (other->minX + other->offset.x)) {
		other->color = { 0.0f,0.0f,0.0f};
		return 0;
	}
	if ((minY+offset.y) > (other->maxY + other->offset.y) || (maxY+offset.y) < (other->minY + other->offset.y)) {
		other->color = { 0.0f,0.0f,0.0f };
		return 0;
	}
	if ((minZ+offset.z) > (other->maxZ + other->offset.z) || (maxZ+offset.z) < (other->minZ + other->offset.z)) {
		other->color = { 0.0f,0.0f,0.0f };
		return 0;
	}
	color = { 1.0f,0.0f,0.0f };
	other->color = { 1.0f,0.0f,0.0f };
	return 1;
}