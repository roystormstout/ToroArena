#pragma once
#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Node.h"
#include "Window.h"

class Curve
{
public:
	glm::mat4x3 control_points;
	std::vector<glm::vec3> curve_verts;
	std::vector<glm::vec3> colors;
	std::vector<unsigned int> indices;
	GLuint VAO, VBO, EBO, Color;
	GLuint uProjection, uModelview;
	Curve(glm::mat4x3 pts);
	~Curve();
	void calculate_verts();
	float Bpolynormial(int i,float time);
	void draw(GLuint shaderProgram);
};

