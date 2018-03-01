#include "Curve.h"



Curve::Curve(glm::mat4x3 pts) : control_points(pts)
{
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
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}


Curve::~Curve()
{
}

void Curve::draw(GLuint shaderProgram)
{
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &Window::V[0][0]);


	glBindVertexArray(VAO);

	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Curve::calculate_verts() {
	for (int i = 0; i < 151;i++) {
		float t = (i* 1.0f)/150.0f;
		float c1 = Bpolynormial(0, t);
		float c2 = Bpolynormial(1, t);
		float c3 = Bpolynormial(2, t);
		float c4 = Bpolynormial(3, t);
		glm::vec4 c = glm::vec4(c1,c2,c3,c4);
		glm::vec3 vert = control_points * c;
		curve_verts.push_back(vert);
		colors.push_back({ 0.0f,0.0f,0.0f });
		if (i < 150) {
			indices.push_back(i);
			indices.push_back(i + 1);
		}
	}
}


float Curve::Bpolynormial(int i, float time) {
	int combination;
	if (i == 1 || i == 2) {
		combination = 3;
	}
	else {
		combination = 1;
	}
	float t_to_i = std::pow(time, i);
	float minus_t_to_i = std::pow(1.0f-time, 3-i);
	return combination * t_to_i*minus_t_to_i;
}

