#pragma once
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class BoundingBox
{
private:
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	GLuint vbo_vertices;
	GLuint ibo_elements;
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;
	glm::vec3 offset;
	glm::mat4 toWorld;
public:
	glm::vec3 color;
	BoundingBox(std::vector<glm::vec3> vertices, glm::vec3 );
	void draw(GLuint shaderProgram);
	~BoundingBox();
	void update(glm::vec3 move);
	int detectCollision(BoundingBox* other);
};