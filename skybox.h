#pragma once
#include "Window.h"
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

class Skybox
{
public:
	GLuint skyboxVAO, skyboxVBO;
	GLuint textureID;
	GLuint uProjection, uModelview;

	Skybox();
	~Skybox();
	void loadCubemap(std::vector<std::string> faces);
	void draw(GLuint skyboxShader);
};

