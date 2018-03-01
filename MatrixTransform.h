#pragma once
#include "Node.h"
#include <list>

class MatrixTransform :
	public Node
{

public:
	float rotate_deg;
	float scale_deg;
	glm::vec3 offset;
	glm::mat4 M;
	std::list<Node*> child_nodes;
	int counter;
	bool left;
	bool right;
	MatrixTransform(glm::mat4, int);
	~MatrixTransform();
	void draw(int program, glm::mat4 C);
	void update();
	void addChild(Node*);
	void walk(float deg);
	void spin(float deg);
	void scale(float deg);
	void translate(glm::vec3 pos);
};

