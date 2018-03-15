#include "Window.h"

#define NONE 0
#define ROTATE 1
#define ZOOM 2
#define TRANSLATE 3

#define LEFT 1
#define RIGHT 2

const char* window_title = "GLFW Project 4";
Cube * cube;
GLint shaderProgram;
Skybox * sky;

int obj_num,mouse_con;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

#define SKY_VERTEX_SHADER_PATH "skybox_shader.vert"
#define SKY_FRAGMENT_SHADER_PATH "skybox_shader.frag"


#define BOX_VERT_SHADER_PATH "box_shader.vert"
#define BOX_FRAG_SHADER_PATH "box_shader.frag"

GLuint skybox_shaderProgram;
GLuint particle_program;

GLuint box_program;
const double m_ROTSCALE = 90.0f;
const double m_TRANSCALE = 2.0f;
const double m_ZOOMSCALE = 0.5f;

Particles* pe;

bool debug;
// Default camera parameters
glm::vec3 Window::cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 Window::cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 Window::cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

glm::vec2 Window::mousePoint;
glm::vec3 Window::lastPoint;
int movement(NONE);

char ox[100] = "C:\\Users\\Lingfeng\\Desktop\\CSE167StarterCode2-master\\Bull_Low_Poly.obj";
char curtain[100] = "C:\\Users\\Lingfeng\\Desktop\\CSE167StarterCode2-master\\curtain2.obj";
char land[100] = "C:\\Users\\Lingfeng\\Desktop\\CSE167StarterCode2-master\\box3.obj";

glm::vec3 p00, p01, p02, p03, p11, p12, p13, p21, p22, p23, p31, p32, p33, p41, p42, p43;

Curve* curve1, *curve2, *curve3, *curve4, *curve5;
Geometry* bull, *ground, *curtain_;
int counter = 0;
void Window::initialize_objects()
{
	sky = new Skybox();
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	skybox_shaderProgram = LoadShaders(SKY_VERTEX_SHADER_PATH, SKY_FRAGMENT_SHADER_PATH);
	box_program = LoadShaders(BOX_VERT_SHADER_PATH, BOX_FRAG_SHADER_PATH);
    particle_program = LoadShaders("Particle.vertexshader", "Particle.fragmentshader");
	glm::mat4 toWorld(1.0f);
	bull = new Geometry(ox, { 1.0f,0.7f,0.5f }, glm::vec3(0.0f, -1.0f, 0.0f), 1);
	ground = new Geometry(land, { 0.95f,0.8f,0.7f },glm::vec3(0.0f,-4.0f,0.0f),0);
	curtain_ = new Geometry(curtain, { 0.9f,0.2f,0.2f },  glm::vec3(0.0f, 0.0f, 10.0f), 1);

	pe = new Particles();
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(sky);
	//delete(cube);
	//glDeleteProgram(shaderProgram);
	glDeleteProgram(skybox_shaderProgram);
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
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 2000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	int detector = bull->box->detectCollision(curtain_->box);
	if (detector == 1) {
		std::cout << "collided~~~~" << std::endl;
		//curtain_->translate(glm::vec3(0, 10, 0));
	}
}

void Window::display_callback(GLFWwindow* window)
{
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	// Clear the color and depth buffers
	glUseProgram(particle_program);
	pe->draw(particle_program);
	// Use the shader of programID
	glUseProgram(shaderProgram);
	bull->draw(shaderProgram);
	ground->draw(shaderProgram);
	curtain_->draw(shaderProgram);
	glUseProgram(box_program);
	bull->box->draw(box_program);
	ground->box->draw(box_program);
	curtain_->box->draw(box_program);
	glUseProgram(skybox_shaderProgram);
	sky->draw(skybox_shaderProgram);

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
	
		if (key == GLFW_KEY_0) {
			if (mouse_con == 1) {
				mouse_con = 0;
			}
			else {
				mouse_con = 1;
			}
		}

		if (key == GLFW_KEY_W) {
			bull->translate(glm::vec3(0.0f, 0.0f, 1.0f));
			pe->update(glm::vec3(0.0f, 0.0f, 1.0f));
		}
		if (key == GLFW_KEY_S) {
			bull->translate(glm::vec3(0.0f, 0.0f, -1.0f));
			pe->update(glm::vec3(0.0f, 0.0f, -1.0f));
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
			float off = 0.3f;
			cam_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rotAxis) * glm::vec4(cam_pos,1.0f));
			V = glm::lookAt(cam_pos, cam_look_at, cam_up);
		}
	}
	lastPoint = curPoint;
}

//resolved
void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (mouse_con == 0) {
		glm::vec3 z_dir = cam_look_at - cam_pos;
			cam_pos -= ((float)-yoffset * glm::normalize(z_dir));
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
	else {
		glm::vec3 z_dir =  -bull->light_dir;
		bull->light_dir -= ((float)(m_ZOOMSCALE * -yoffset) * glm::normalize(z_dir));
	}
}