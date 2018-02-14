#pragma warning(disable : 4996)
#include "Light.h"
#include "glm/ext.hpp"
#include "Window.h"

Light::Light() {

}

Light::Light(int mode)
{
	light_color = { 1.0f,1.0f,1.0f };
	toWorld = mat4(1.0f);
	light_mode = mode;
	if (mode == 1) {// direct light
		//light_dir = { -1.0f, -1.0f, 0.0f };
		light_dir = { 0.0f,  -1.0f, -1.0f };
		// to rotate with matrix, 0.0f
	}
	else if (mode == 2) {// point light
		light_pos = { 0.0f, 0.0f, 5.0f };
		cons_att = 1.0f;
		linear_att = 0.07f;
		quad_att = 0.017f;

	}
	else if (mode == 3) { // spot light
		light_dir = { 0.0f, -1.0f, -1.0f };
		light_pos = { 0.0f, 3.0f, 3.0f };
		//cutoff_angle = glm::cos(glm::radians(7.5f));
		exponent = 0.4f;
		cons_att = 1.0f;
		linear_att = 0.07f;
		quad_att = 0.01f;

	}

}


Light::~Light()
{
}



void Light::update() {
		light_dir = glm::vec3(glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), { 0.0f, 1.0f, 0.0f }) * glm::vec4(light_dir, 0.0f));
}