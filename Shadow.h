#pragma once
#ifndef SHADOW_H
#define SHADOW_H
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

class Shadow {
private:
	glm::mat4 lightSpaceMatrix;
	unsigned int depthMapFBO;
	unsigned int depthMap;

public:
	Shadow();
	void first_pass(GLuint shaderProgram);
	void second_pass(GLuint shaderProgram);
	void renderScene(GLuint shaderProgram);

};

#endif