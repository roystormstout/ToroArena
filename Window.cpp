#include "window.h"

#define NONE 0
#define ROTATE 1
#define ZOOM 2
#define TRANSLATE 3

#define LEFT 1
#define RIGHT 2

const char* window_title = "GLFW Project 3";
Cube * cube;
GLint shaderProgram;
Skybox * sky;

int obj_num,mouse_con;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "shader.vert"
#define FRAGMENT_SHADER_PATH "shader.frag"

#define SKY_VERTEX_SHADER_PATH "skybox_shader.vert"
#define SKY_FRAGMENT_SHADER_PATH "skybox_shader.frag"
GLint skybox_shaderProgram;

const double m_ROTSCALE = 90.0f;
const double m_TRANSCALE = 2.0f;
const double m_ZOOMSCALE = 0.5f;


// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 50.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

glm::vec2 Window::mousePoint;
glm::vec3 Window::lastPoint;
int movement(NONE);

char body[100] = "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\robot-parts\\body.obj";
char antenna[100] = "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\robot-parts\\antenna.obj";
char eyeball[100] = "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\robot-parts\\eyeball.obj";
char head[100] = "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\robot-parts\\head.obj";
char limb[100] = "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\robot-parts\\limb.obj";

MatrixTransform * army;

glm::vec3 p00, p01, p02, p03, p11, p12, p13, p21, p22, p23, p31, p32, p33, p41, p42, p43;

Curve* curve1, *curve2, *curve3, *curve4, *curve5;

int counter = 0;
void Window::initialize_objects()
{
	sky = new Skybox();
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	skybox_shaderProgram = LoadShaders(SKY_VERTEX_SHADER_PATH, SKY_FRAGMENT_SHADER_PATH);
	glm::mat4 toWorld(1.0f);
	army = new MatrixTransform(toWorld,NONE);
	for (int i = 0;i <5;i++) {
		for (int j = 0; j < 5;j++) {
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(140-70.0f*i, 0.0f, 140-70.0f*j));
			army->addChild(createBot(trans*toWorld));
		}
	}
	p00 = { 0.0f,0.0f,-100.0f };
	p01 = { 0.0f,100.0f,-100.0f };
	p02 = { 100.0f,100.0f,-100.0f };
	p03 = { 100.0f,0.0f,-100.0f };
	glm::mat4x3 controlPts0 = { p00,p01,p02,p03 };
	curve1 = new Curve(controlPts0);

	//Bezier Curve 1
	p11 = { 100.0f,-100.0f,-100.0f };
	p12 = { 200.0f,-100.0f,0.0f };
	p13 = { 200.0f,0.0f,0.0f };
	glm::mat4x3 controlPts1 = { p03,p11,p12,p13 };
	curve2 = new Curve(controlPts1);

	//Bezier Curve 2
	p21 = { 200.0f,100.0f,0.0f };
	p22 = { 300.0f,100.0f,100.0f };
	p23 = { 300.0f,0.0f,100.0f };
	glm::mat4x3 controlPts2 = { p13,p21,p22,p23 };
	curve3 = new Curve(controlPts2);

	//Bezier Curve 3
	p31 = { 200.0f,-100.0f,100.0f };
	p32 = { 200.0f,-100.0f,200.0f };
	p33 = { 200.0f,0.0f,100.0f };
	glm::mat4x3 controlPts3 = { p23,p31,p32,p33 };
	curve4 = new Curve(controlPts3);

	//Bezier Curve 4
	p41 = { 200.0f,100.0f,-100.0f };
	p42 = { 100.0f,100.0f,0.0f };
	glm::mat4x3 controlPts4 = { p33,p41,p42,p00 };
	curve5 = new Curve(controlPts4);


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
	// Call the update function the cube
	glUseProgram(shaderProgram);
	army->update();
	std::cout << counter << std::endl;
	if (counter < 150) {
		army->translate(curve1->curve_verts[counter]);
	}	
	else if (counter < 300) {
		army->translate(curve2->curve_verts[counter-150]);
	}
	else if (counter < 450) {
		army->translate(curve3->curve_verts[counter - 300]);
	}
	else if (counter < 600) {
		army->translate(curve4->curve_verts[counter - 450]);
	}
	else if (counter < 750) {
		army->translate(curve5->curve_verts[counter - 600]);
	}
	else {
		counter = 0;
	}
	counter++;
}

void Window::display_callback(GLFWwindow* window)
{
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	army->draw(shaderProgram, glm::mat4(1.0f));
	curve1->draw(shaderProgram);
	curve2->draw(shaderProgram);
	curve3->draw(shaderProgram);
	curve4->draw(shaderProgram);
	curve5->draw(shaderProgram);
	//robotBody->draw(shaderProgram, glm::mat4(1.0f));
	// Use the shader of programID
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
		if (key == GLFW_KEY_S) {
			army->scale(1.2f);
		}
		if (key == GLFW_KEY_W) {
			army->scale(0.8f);
		}
	}
	if (action != GLFW_RELEASE) {
		if (key == GLFW_KEY_A) {
			army->spin(-2.0f);
		}
		if (key == GLFW_KEY_D) {
			army->spin(2.0f);
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
	glm::vec3 z_dir = cam_look_at - cam_pos;
	cam_pos -= ((float)-yoffset * glm::normalize(z_dir));
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

MatrixTransform * Window::createBot(glm::mat4 matrix) {
	MatrixTransform * robotMatrix;
	glm::mat4 toWorld;
	glm::vec3 rotAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	
	//main control
	robotMatrix = new MatrixTransform(matrix, NONE);

	//body
	Geometry* robotBody = new Geometry(body);
	toWorld = glm::rotate(glm::mat4(1.0f), -90.0f / 180.0f * glm::pi<float>(), rotAxis)*glm::mat4(1.0f);
	MatrixTransform * bodyTransform = new MatrixTransform(toWorld, NONE);
	bodyTransform->addChild(robotBody);
	robotMatrix->addChild(bodyTransform);

	//head
	Geometry* robotHead = new Geometry(head);
	Geometry* leftrobotEye = new Geometry(eyeball);
	Geometry* rightrobotEye = new Geometry(eyeball);
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f))*toWorld;
	MatrixTransform* headTransform = new MatrixTransform(toWorld, NONE);
	//eyes
	glm::mat4 eyeWorld1 = glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, -5.0f, 90.0f));
	MatrixTransform* eyeTransform = new MatrixTransform(eyeWorld1, NONE);
	glm::mat4 eyeWorld2 = glm::translate(glm::mat4(1.0f), glm::vec3(-7.0f, -5.0f, 90.0f));
	MatrixTransform* eyeTransform2 = new MatrixTransform(eyeWorld2, NONE);
	headTransform->addChild(robotHead);
	eyeTransform->addChild(leftrobotEye);
	eyeTransform2->addChild(rightrobotEye);
	headTransform->addChild(eyeTransform);
	headTransform->addChild(eyeTransform2);
	//antenna
	Geometry* leftrobotAntenna = new Geometry(antenna);
	glm::mat4 leftantennaMat = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, -5.0f, 90.0f));
	MatrixTransform* antennaTrans = new MatrixTransform(leftantennaMat, NONE);
	antennaTrans->addChild(leftrobotAntenna);
	headTransform->addChild(antennaTrans);

	robotMatrix->addChild(headTransform);

	//left arm 
	Geometry* leftArm = new Geometry(limb);
	toWorld = glm::rotate(glm::mat4(1.0f), 120.0f / 180.0f * glm::pi<float>(), rotAxis)*glm::mat4(1.0f);
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f))*toWorld;
	MatrixTransform* leftArmTransform = new MatrixTransform(toWorld, RIGHT);
	leftArmTransform->addChild(leftArm);


	//right arm 
	Geometry* rightArm = new Geometry(limb);
	toWorld = glm::rotate(glm::mat4(1.0f), 60.0f / 180.0f * glm::pi<float>(), rotAxis)*glm::mat4(1.0f);
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 0.0f, 0.0f))*toWorld;
	MatrixTransform* rightArmTransform = new MatrixTransform(toWorld, LEFT);
	rightArmTransform->addChild(rightArm);


	MatrixTransform* armTransform = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,110.0f, 0.0f)), NONE);
	armTransform->addChild(rightArmTransform);
	armTransform->addChild(leftArmTransform);
	robotMatrix->addChild(armTransform);

	//left leg 
	Geometry* leftLeg = new Geometry(limb);
	toWorld = glm::mat4(1.0f);
	toWorld = glm::rotate(glm::mat4(1.0f), 60.0f / 180.0f * glm::pi<float>(), rotAxis)*glm::mat4(1.0f);
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(15.0f, 0.0f, 0.0f))*toWorld;
	MatrixTransform* leftLegTransform = new MatrixTransform(toWorld,LEFT);
	leftLegTransform->addChild(leftLeg);


	//right leg 
	Geometry* rightLeg = new Geometry(limb);
	toWorld = glm::mat4(1.0f);
	toWorld = glm::rotate(glm::mat4(1.0f), 120.0f / 180.0f * glm::pi<float>(), rotAxis)*glm::mat4(1.0f);
	toWorld = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 0.0f, 0.0f))*toWorld;
	MatrixTransform* rightLegTransform = new MatrixTransform(toWorld, RIGHT);
	rightLegTransform->addChild(rightLeg);

	MatrixTransform* legTransform = new MatrixTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 80.0f, 0.0f)), NONE);
	legTransform->addChild(rightLegTransform);
	legTransform->addChild(leftLegTransform);
	robotMatrix->addChild(legTransform);
	return robotMatrix;
}