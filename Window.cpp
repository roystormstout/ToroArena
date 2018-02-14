#include "window.h"

#define NONE 0
#define ROTATE 1
#define ZOOM 2
#define TRANSLATE 3


const char* window_title = "GLFW Starter Project";
Cube * cube;
GLint shaderProgram;
OBJObject * obj, * obj2, * obj3;

int obj_num,mouse_con;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "../shader.vert"
#define FRAGMENT_SHADER_PATH "../shader.frag"

#define LIGHT_VERTEX_SHADER_PATH "../light_shader.vert"
#define LIGHT_FRAGMENT_SHADER_PATH "../light_shader.frag"
GLint light_shaderProgram;

const double m_ROTSCALE = 90.0f;
const double m_TRANSCALE = 2.0f;
const double m_ZOOMSCALE = 0.5f;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

glm::vec2 Window::mousePoint;
glm::vec3 Window::lastPoint;
int movement(NONE);
int normal_coloring = 1;
glm::vec3 color_spec0 = glm::vec3(0.992157f, 0.941176f, 0.807843f); // bunny is all shiny no diffuse color
glm::vec3 color_diff0 = glm::vec3(0,0,0);
glm::vec3 color_ambi0 = glm::vec3(0.329412f, 0.223529f, 0.027451f); // brass

glm::vec3 color_spec1 = glm::vec3(0, 0, 0);
glm::vec3 color_diff1 = glm::vec3(0.61424f, 0.04136f, 0.04136f); // dragon is all diffuse no shininess
glm::vec3 color_ambi1 = glm::vec3(0.1745f, 0.01175f, 0.01175f); // ruby

glm::vec3 color_ambi2 = glm::vec3(0.24725f, 0.1995f, 0.0745f); // bear should have significant diffuse and specular reflection components
glm::vec3 color_diff2 = glm::vec3(0.75164f, 0.60648f, 0.22648f);
glm::vec3 color_spec2 = glm::vec3(0.628281f, 0.555802f, 0.366065f); // gold

void Window::initialize_objects()
{
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	light_shaderProgram = LoadShaders(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);
	obj = new OBJObject("C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\bunny.obj",
		color_spec0,
		color_diff0,
		color_ambi0
		);
	obj2 = new OBJObject("C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\dragon.obj",
		color_spec1,
		color_diff1,
		color_ambi1
	);
	obj3 = new OBJObject("C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\bear.obj",
		color_spec2,
		color_diff2,
		color_ambi2
	);

	obj_num = 1;
	normal_coloring = 1;
	mouse_con = 0;
	// Load the shader program. Make sure you have the correct filepath up top
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
	delete(obj);
	delete(obj2);
	delete(obj3);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(light_shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	if (obj_num == 1)
		obj->update();
	else if (obj_num == 2)
		obj2->update();
	else if (obj_num == 3)
		obj3->update();


}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render the 
	if (obj_num == 1) {
		obj->draw(shaderProgram);
		glUseProgram(light_shaderProgram);
		if (obj->light_mode == 2) {
			obj->draw_light(light_shaderProgram,2);
		}
		else if (obj->light_mode == 3) {
			obj->draw_light(light_shaderProgram,3);
		}
	}
	else if (obj_num == 2) {
		obj2->draw(shaderProgram);
		glUseProgram(light_shaderProgram);
		if (obj2->light_mode == 2) {
			obj2->draw_light(light_shaderProgram,2);
		}
		else if (obj2->light_mode == 3) {
			obj2->draw_light(light_shaderProgram,3);
		}
	}
	else if (obj_num == 3) {
		obj3->draw(shaderProgram);
		glUseProgram(light_shaderProgram);
		if (obj3->light_mode == 2) {
			obj3->draw_light(light_shaderProgram,2);
		}
		else if (obj3->light_mode == 3) {
			obj3->draw_light(light_shaderProgram,3);
		}
	}
	//cube->draw(shaderProgram);
	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "coloring_mode"), normal_coloring);
	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_S) {
			if (obj_num == 1) {
				if (mods == GLFW_MOD_SHIFT)
					obj->scale_obj(1.2f);
				else obj->scale_obj(0.8f);
			}			
			if (obj_num == 2) {
				if (mods == GLFW_MOD_SHIFT)
					obj2->scale_obj(1.2f);
				else obj2->scale_obj(0.8f);
			}
			if (obj_num == 3) {
				if (mods == GLFW_MOD_SHIFT)
					obj3->scale_obj(1.2f);
				else obj3->scale_obj(0.8f);
			}
		}
		if (key == GLFW_KEY_F1) {
			obj_num = 1;
		}
		if (key == GLFW_KEY_F2) {
			obj_num = 2;
		}
		if (key == GLFW_KEY_F3) {
			obj_num = 3;
		}
		if (key == GLFW_KEY_0) {
			if (mouse_con == 1) {
				mouse_con = 0;
			}
			else {
				mouse_con = 1;
			}
		}
		if (key == GLFW_KEY_N) {
			if (normal_coloring==1) {
				normal_coloring = 0;
				glUniform1i(glGetUniformLocation(shaderProgram, "coloring_mode"), normal_coloring);
			}
			else {
				normal_coloring = 1;
				glUniform1i(glGetUniformLocation(shaderProgram, "coloring_mode"), normal_coloring);
			}
		}
		if (key == GLFW_KEY_E) {
			if (obj_num == 1) {
				if (mods == GLFW_MOD_SHIFT) {
					obj->exponent *= 2.0f;
				}
				else {
					if(obj->exponent>0.3f)
						obj->exponent /= 2.0f;
				}
			}
			if (obj_num == 2) {
				if (mods == GLFW_MOD_SHIFT) {
					obj2->exponent *= 2.0f;
				}
				else {
					if (obj2->exponent > 0.3f)
						obj2->exponent /= 2.0f;
				}
			}
			if (obj_num == 3) {
				if (mods == GLFW_MOD_SHIFT) {
					obj3->exponent *= 2.0f;
				}
				else {
					if (obj3->exponent>0.3f)
						obj3->exponent /= 2.0f;
				}
			}
		}
		if (key == GLFW_KEY_W) {
			if (obj_num == 1) {
				if (mods == GLFW_MOD_SHIFT) {
					obj->sptLightOut += 1.0f;
				}
				else {
					if (obj->sptLightOut > 2.0f) {
						obj->sptLightOut -= 1.0f;
					}
				}
			}
			if (obj_num == 2) {
				if (mods == GLFW_MOD_SHIFT) {
					obj2->sptLightOut += 1.0f;
				}
				else {
					if (obj2->sptLightOut > 2.0f) {
						obj2->sptLightOut -= 1.0f;
					}
				}
			}
			if (obj_num == 3) {
				if (mods == GLFW_MOD_SHIFT) {
					obj3->sptLightOut += 1.0f;
				}
				else {
					if (obj3->sptLightOut > 2.0f) {
						obj3->sptLightOut -= 1.0f;
					}
				}
			}
		}
		if (key == GLFW_KEY_1) {
			if (obj_num == 1)
				 obj->light_mode = 1;
			if (obj_num == 2) 
				obj2->light_mode = 1;
			if (obj_num == 3) 
				obj3->light_mode = 1;
		}
		if (key == GLFW_KEY_2) {
			if (obj_num == 1)
				obj->light_mode = 2;
			if (obj_num == 2)
				obj2->light_mode = 2;
			if (obj_num == 3)
				obj3->light_mode = 2;
		}
		if (key == GLFW_KEY_3) {
			if (obj_num == 1)
				obj->light_mode = 3;
			if (obj_num == 2)
				obj2->light_mode = 3;
			if (obj_num == 3)
				obj3->light_mode = 3;
		}
		if (key == GLFW_KEY_4) {
			if (obj_num == 1) {
				if (obj->dir_light_on == 1) {
					obj->dir_light_on = 0;
				}
				else {
					obj->dir_light_on = 1;
				}
			}
			if (obj_num == 2) {
				if (obj2->dir_light_on == 1) {
					obj2->dir_light_on = 0;
				}
				else {
					obj2->dir_light_on = 1;
				}
			}
			if (obj_num == 3) {
				if (obj3->dir_light_on == 1) {
					obj3->dir_light_on = 0;
				}
				else {
					obj3->dir_light_on = 1;
				}
			}

		}
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	glm::vec2 point = glm::vec2(mousePoint);

	if (action == GLFW_PRESS){
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			movement = ROTATE;
			// Map the mouse position to a logical sphere location.
			// Keep it in the class variable lastPoint.
			lastPoint = trackBallMapping(point);
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			movement = TRANSLATE;
			// Map the mouse position to a logical sphere location.
			// Keep it in the class variable lastPoint.
			lastPoint = trackBallMapping(point);
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			movement = ZOOM;
			// Map the mouse position to a logical sphere location.
			// Keep it in the class variable lastPoint.
			lastPoint = trackBallMapping(point);
		}
	}
	if (action == GLFW_RELEASE) {
		movement = NONE;
	}
}

glm::vec3 Window::trackBallMapping(glm::vec2 point)
{
	glm::vec3 v;
	float d;
	v.x = (2.0*point.x - width) / width;
	v.y = (height - 2.0*point.y) / height;
	v.z = 0.0;
	d = glm::length(v);
	d = (d<1.0) ? d : 1.0;
	v.z = sqrtf(1.001 - d * d);
	v = glm::normalize(v); // Still need to normalize, since we only capped d, not v.
	return v;
}

void Window::cursor_movement_callback(GLFWwindow* window, double x, double y)
{
	// 
	// Handle any necessary mouse movements
	//
	glm::vec3 direction;
	float rot_angle;
	glm::vec3 curPoint;

	mousePoint = { x, y };
	if (mouse_con == 1) {
		if (movement == ROTATE) // Left-mouse button is being held down
		{
			glm::vec2 point = glm::vec2(x, y);
			curPoint = trackBallMapping(point); // Map the mouse position to a logical
												// sphere location.
			direction = curPoint - lastPoint;
			float velocity = glm::length(direction);
			if (velocity > 0.01) // If little movement - do nothing.
			{
				//get axis to rotate around
				glm::vec3 rotAxis;
				rotAxis = glm::cross(lastPoint, curPoint);
				rot_angle = velocity * m_ROTSCALE;
				if (obj_num == 1)
					obj->rotate_obj(rot_angle, rotAxis);
				else if (obj_num == 2)
					obj2->rotate_obj(rot_angle, rotAxis);
				else if (obj_num == 3)
					obj3->rotate_obj(rot_angle, rotAxis);
			}

		}
		else if (movement == TRANSLATE)
		{
			glm::vec2 point = glm::vec2(x, y);
			curPoint = trackBallMapping(point); // Map the mouse position to a logical
												// sphere location.
			direction = curPoint - lastPoint;
			float velocity = glm::length(direction);
			direction.z = 0;
			if (velocity > 0.001) // If little movement - do nothing.
			{
				direction = { direction.x * m_TRANSCALE, direction.y * m_TRANSCALE, 0 };
				if (obj_num == 1)
					obj->translate_obj(direction);
				else if (obj_num == 2)
					obj2->translate_obj(direction);
				else if (obj_num == 3)
					obj3->translate_obj(direction);
			}
		}
	}
	else {
		if (movement == ROTATE) {
			glm::vec2 point = glm::vec2(x, y);
			curPoint = trackBallMapping(point); // Map the mouse position to a logical
												// sphere location.
			direction = curPoint - lastPoint;
			float velocity = glm::length(direction);
			if (velocity > 0.01) // If little movement - do nothing.
			{
				//get axis to rotate around
				glm::vec3 rotAxis;
				rotAxis = glm::cross(lastPoint, curPoint);
				rot_angle = velocity * m_ROTSCALE;
				if (obj_num == 1)
					obj->rotate_light(rot_angle, rotAxis);
				else if (obj_num == 2)
					obj2->rotate_light(rot_angle, rotAxis);
				else if (obj_num == 3)
					obj3->rotate_light(rot_angle, rotAxis);
			}
		}
	}
	lastPoint = curPoint;
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (obj_num == 1)
		obj->translate_obj({ 0,0, -1 * yoffset * m_ZOOMSCALE });
	if (obj_num == 2)
		obj2->translate_obj({ 0,0, -1 * yoffset * m_ZOOMSCALE });
	else if (obj_num == 3)
		obj3->translate_obj({ 0,0, -1 * yoffset * m_ZOOMSCALE });
}