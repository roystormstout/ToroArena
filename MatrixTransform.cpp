#include "MatrixTransform.h"


glm::vec3 lastPos;
MatrixTransform::MatrixTransform(glm::mat4 m, int part): M(m)
{
	rotate_deg = 0.0f;
	scale_deg = 1.0f;
	offset = { 0.0f,0.0f,0.0f };
	if (part == 0) {
		left = false;
		right = false;
	}
	//left
	else if (part == 1) {
		left = true;
		right = false;
	}
	//right
	else if (part == 2) {
		left = false;
		right = true;
	}

}


MatrixTransform::~MatrixTransform()
{
}

void MatrixTransform::draw(int program, glm::mat4 C)
{
	auto MT = C * M;
	for (auto i : child_nodes) {
		//std::cout << "mt draw " << count << std::endl;
		i->draw(program, MT);
	}
}

void MatrixTransform::update()
{
	for (auto i : child_nodes) {
		//std::cout << "mt draw " << count << std::endl;
		i->update();
	}
	if (counter < 40) {
		if(left)
			walk(0.02f);
		else if(right)
			walk(-0.02f);
		counter++;
	}
	else {
		if (left)
			walk(-0.02f);
		else if (right)
			walk(0.02f);
		counter++;
	}
	if (counter == 80) counter = 0;
}

void MatrixTransform::walk(float deg) {
	//    M = M * glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
	//    M = M * glm::rotate(glm::mat4(1.0f), sin(deg), glm::vec3(1.0f, 0.0f, 0.0f));

	M = glm::rotate(glm::mat4(1.0f), sin(deg), glm::vec3(1.0f, 0.0f, 0.0f)) * M;
}

void MatrixTransform::addChild(Node* child) {
	child_nodes.push_back(child);
}

void MatrixTransform::spin(float deg)
{
	rotate_deg += deg;
	// If you haven't figured it out from the last project, this is how you fix spin's behavior
	M = M*glm::rotate(glm::mat4(1.0f), deg / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void MatrixTransform::scale(float deg) {
	M = M*glm::scale(glm::mat4(1.0f), glm::vec3(deg, deg, deg));
}

void MatrixTransform::translate(glm::vec3 pos) {
	glm::vec3 off= pos - lastPos;
	offset += off;
	M = glm::translate(glm::mat4(1.0f), off)*M;
	lastPos = pos;
}