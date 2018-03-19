#include "DoF.h"

using namespace std;
using namespace glm;

GLuint frameBuffer;
GLuint colormap;
GLuint depthmap;
GLuint dofVAO, dofVBO;

// Quad vertices
GLfloat quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f,  1.0f,  1.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

// need to be called after the framebuffer has been created
void createColorDepthMap() {
	glGenTextures(1, &depthmap);
	glGenTextures(1, &colormap);

	// DEPTH
	glBindTexture(GL_TEXTURE_2D, depthmap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Window::width, Window::height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0
	);

	// COLOR
	glBindTexture(GL_TEXTURE_2D, colormap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, Window::width, Window::height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colormap, 0
	);
}


void createFrameBuffer() {
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
}

DoF::DoF(GLuint program) {

	aperture = 2.0f;
	focus = 1.0f;
	max_blur = 1.0f;

	// create the frame buffer and depth buffer color buffer
	glGenVertexArrays(1, &dofVAO);
	glGenBuffers(1, &dofVBO);

	glBindVertexArray(dofVAO);
	glBindBuffer(GL_ARRAY_BUFFER, dofVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	GLint posAttrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(program, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// create them
	createFrameBuffer();
	createColorDepthMap();

	// check if it is completed
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err == GL_FRAMEBUFFER_COMPLETE) {
		cout << "frame buffer is completed" << endl;
	}

}

DoF::~DoF() {}

void DOF_Postprocessing(GLuint program, float aperture, float focus, float maxBlur) {

	glUseProgram(program);
	glBindVertexArray(dofVAO);

	glUniform1f(glGetUniformLocation(program, "maxBlur"), maxBlur);
	glUniform1f(glGetUniformLocation(program, "aperture"), aperture);
	glUniform1f(glGetUniformLocation(program, "focus"), focus);
	glUniform1f(glGetUniformLocation(program, "aspect"), Window::width / (float)Window::height);

	//cout << "the sizes are " << Window::width << " and " << Window::height << " and " << Window::width / (float)Window::height << endl;

	// bindColormap
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colormap);

	// bind ddepthmap
	glUniform1i(glGetUniformLocation(program, "tDepth"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);

}

void DoF::bindFrameBuffer() {
	// use the customized framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glBindTexture(GL_TEXTURE_2D, depthmap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, Window::width, Window::height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0
	);

	// COLOR
	glBindTexture(GL_TEXTURE_2D, colormap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, Window::width, Window::height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL
	);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colormap, 0
	);
}

void DoF::dof_post_processing(GLuint program) {
	// unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// DOF post processing
	DOF_Postprocessing(program, aperture, focus, max_blur);
}

// controls
void DoF::increase_focus() {
	// cap to 1
	float temp = focus + 0.001f;

	focus = temp;
	cout << "focus is " << focus << endl;
}

void DoF::decrease_focus() {
	// cap to 1
	float temp = focus - 0.001f;
	focus = temp;

	cout << "focus is " << focus << endl;
}

void DoF::reset_focus() {
	focus = 1.0f;
}

void DoF::increase_aperture() {
	aperture += 0.1f;

	cout << "aperture is " << aperture << endl;
}

void DoF::decrease_aperture() {
	aperture -= 0.1f;
	cout << "aperture is " << aperture << endl;
}