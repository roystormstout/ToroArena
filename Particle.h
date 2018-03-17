#ifndef _PARTICLE_
#define _PARTICLE_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include "glm/ext.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <math.h>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <random>


class Particles {

public:
	glm::vec3 translation;
	Particles(GLuint);
	~Particles();
	void update(glm::vec3 move);
	/// better be called before swapping buffer
	void draw(GLuint);
	GLuint VertexArrayID, Texture;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	// controls
};

#endif