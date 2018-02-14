#ifndef OBJOBJECT_H
#define OBJOBJECT_H

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


class OBJObject
{
private:
std::vector<unsigned int> indices;

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> colors;
std::vector<glm::vec3> normals;


std::vector<unsigned int> indices_s;

std::vector<glm::vec3> vertices_s;
std::vector<glm::vec3> normals_s;

std::vector<unsigned int> indices_p;

std::vector<glm::vec3> vertices_p;
std::vector<glm::vec3> normals_p;

public:
	glm::mat4 toWorld;


	glm::vec3 offset, spec, diffuse, ambi;
	glm::mat4 rotateMat;
	glm::mat4 translateMat;
	glm::mat4 scaleMat;
	int light_mode, dir_light_on;
	float sptLightOut;
	float exponent;
	float sptLightIn;

	OBJObject(const char *file1, glm::vec3 s, glm::vec3 d, glm::vec3 a);
	OBJObject();
	~OBJObject();
	void update();
	void reset();
	void rotate_obj(float rot_angle, glm::vec3 rotAxis);
	void scale_obj(float scale);
	void translate_obj(glm::vec3 direction);
	void parse(const char *file1);
	void draw(GLuint shaderProgram);
	void rotate_light(float rot_angle, glm::vec3 rotAxis);
	void draw_light(GLuint shaderProgram, int light_num);

	void parse_light(const char* filepath, const char* filepath2);
	void setModernOpenGL();
	GLuint Light_VBO_vertices, Light_VBO_normals, Light_VAO, Light_EBO;
	GLuint Light_VBO_vertices2, Light_VBO_normals2, Light_VAO2, Light_EBO2;
	GLuint uLight_type, uLight_position, uLight_color, uLight_constant;

	// These variables are needed for the shader program
	GLuint VBO, VAO, NBO, EBO;
	GLuint uProjection, uModelview, uModel, uView;
};

#endif