#pragma once
#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "Window.h"

class Plant
{
public:
	float angle;
	float lineWidth;
	std::vector<glm::vec3> curve_verts;
	std::vector<glm::vec3> colors;
	std::vector<unsigned int> indices;
	GLuint VAO, VBO, EBO, Color;
	GLuint uProjection, uModelview;
	glm::vec3 startPoint;
	glm::vec3 currentColor;
	glm::vec3 startAngle;
	std::string ruleX;
	std::string ruleF;
	std::string startStr;
	Plant();
	Plant(glm::vec3 sp, std::string X, std::string F, float);
	Plant(glm::vec3 sp, std::string X, std::string F, float ang, std::string axiom);
	~Plant();
	void calculate_verts();
	void draw(GLuint shaderProgram);
};

