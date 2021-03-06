#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include "Cube.h"
#include "shader.h"
#include "skybox.h"
#include "Node.h"
#include "MatrixTransform.h"
#include "Geometry.h"
#include "Curve.h"
#include "Particle.h"
#include "DoF.h"
#include "Plant.h"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P; // P for projection
	static glm::mat4 V; // V for view
	static glm::vec3 lastPoint;
	static glm::vec2 mousePoint;
	static glm::vec3 cam_pos;
	static glm::vec3 cam_look_at;
	static glm::vec3 cam_up;
	static void initialize_objects();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_movement_callback(GLFWwindow* window, double x, double y);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static glm::vec3 trackBallMapping(glm::vec2 point);
};

#endif
