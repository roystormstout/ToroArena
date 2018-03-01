#pragma once
#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Node.h"
#include "Window.h"

class Geometry :
	public Node
{
private:
	glm::mat4 modelview;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<GLuint> indices;
	std::vector<glm::vec3> normals;

public:
	GLuint VAO, VBO, EBO, color;
	GLuint uProjection, uModelview, uTrans;
	Geometry(const char *filepath);
	~Geometry();

	void draw(int program, glm::mat4 M);
	void parse(const char *filepath);
	void setup();
	void update();
};

