#ifndef LIGHT_H
#define LIGHT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace std;
using namespace glm;
class Light
{
private:
	std::vector<unsigned int> indices;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec3> normals;

public:

	int light_mode;

	glm::vec3 light_color;
	glm::vec3 light_pos;
	glm::vec3 light_dir;

	float cons_att;
	float linear_att;
	float quad_att;

	glm::mat4 toWorld;
	float cutoff_angle;
	float exponent;
	Light();
	Light(int mode);
	~Light();

	void update();

};

#endif