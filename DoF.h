#pragma once
#include <GL/glew.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Node.h"
#include "Window.h"

class DoF {

public:
	DoF(GLuint);
	~DoF();

	/// need to be called at the front of the thing that you want to post processing
	void bindFrameBuffer();
	/// better be called before swapping buffer
	void dof_post_processing(GLuint screenShaderProgram);

	// controls
	void increase_aperture();
	void decrease_aperture();
	void reset_aperture();

	void increase_focus();
	void decrease_focus();
	void reset_focus();

	void increase_blur();
	void decrease_blur();
	void reset_blur();


	float aperture, focus, max_blur;
};