#include "Plant.h"
#include <stdio.h>      /* printf */
#include <math.h>       /* cos */
Plant::Plant()
{
    startStr = "X";
	startPoint = { -16.0f,-1.0f, 60.0f };
	startAngle = { 0,0.1f,0 };
	ruleX = "F-[+FX]+F[-XFX]+F[-FX]";
	ruleF = "FF";
	angle = 45.0f;
	lineWidth = 3.0f;
	currentColor = { 0.15f,0.3f,0 };
	calculate_verts();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &Color);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, curve_verts.size() * sizeof(glm::vec3), &curve_verts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, Color);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Plant::Plant(glm::vec3 sp, std::string X, std::string F, float ang, glm::vec3 color)
{
	lineWidth = 2.0f;
	startStr = "X";
	startPoint = sp;
	startAngle = { 0,0.1f,0 };
	ruleX = X;
	ruleF = F;
	angle = ang;
	currentColor =  color;
	calculate_verts();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &Color);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, curve_verts.size() * sizeof(glm::vec3), &curve_verts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, Color);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

Plant::Plant(glm::vec3 sp, std::string X, std::string F, float ang, std::string axiom)
{

	lineWidth = 1.0f;
	startStr = axiom;
	startPoint = sp;
	startAngle = { 0,0.05f,0 };

	ruleX = X;
	ruleF = F;
	angle = ang;
	currentColor = { 0.6f,0.55f,0.4f };
	calculate_verts();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &Color);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, curve_verts.size() * sizeof(glm::vec3), &curve_verts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, Color);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}


Plant::~Plant()
{
}

void Plant::draw(GLuint shaderProgram)
{
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &Window::V[0][0]);


	glBindVertexArray(VAO);
	glLineWidth(lineWidth);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Plant::calculate_verts() {
	std::vector<std::pair<glm::vec3, glm::vec3>> stack;
	curve_verts.push_back(startPoint);
	for (int i = 0; i < 6;i++) {
		std::string nextStr = "";
		for (int j = 0; j < startStr.length(); j++) {
			if (startStr[j] == 'X') {
				nextStr = nextStr.append(ruleX);
			}
			else if (startStr[j] == 'F') {
				nextStr = nextStr.append(ruleF);
			}
			else {
				nextStr.push_back(startStr[j]);
			}
		}
		startStr = nextStr;
	}
	for (int j = 0; j < startStr.length(); j++) {
		if (startStr[j] == 'F') {
			startPoint += startAngle;
			curve_verts.push_back(startPoint);
			colors.push_back(currentColor);
			indices.push_back(curve_verts.size() - 2);
			indices.push_back(curve_verts.size() - 1);
		}
		if (startStr[j] == '-') {
			float x = startAngle.x;
			float y = startAngle.y;
			startAngle.x = x * cos(-angle) - y * sin(-angle);
			startAngle.y = x * sin(-angle) + y * cos(-angle);
			startAngle.z = ((float)(rand()%2000-1000)/2000.0f)*(2*x);

		}
		if (startStr[j] == '+') {
			float x = startAngle.x;
			float y = startAngle.y;
			startAngle.x = x * cos(angle) - y * sin(angle);
			startAngle.y = x * sin(angle) + y * cos(angle);
			startAngle.z = ((float)(rand() % 2000 - 1000) / 2000.0f)*(2 * x);
		}
		if (startStr[j] == '[') {
			stack.push_back(std::pair<glm::vec3, glm::vec3>(startAngle, startPoint));
			currentColor += glm::vec3(0.03f, 0.07f, 0.01f);
		}
		if (startStr[j] == ']') {
			currentColor -= glm::vec3(0.03f, 0.07f, 0.01f);
			std::pair<glm::vec3, glm::vec3> restored = stack.back();
			stack.pop_back();
			startAngle = restored.first;
			startPoint = restored.second;
			curve_verts.push_back(startPoint);
			colors.push_back(currentColor);
		}
	}
	//std::cout << startStr << std::endl;
}


