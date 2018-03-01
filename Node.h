#pragma once
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

class Node
{
public:
	virtual void draw(int program, glm::mat4 C) = 0;
	virtual void update() = 0;
};

