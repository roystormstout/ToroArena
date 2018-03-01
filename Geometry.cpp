#pragma warning(disable : 4996)
#include "Geometry.h"



Geometry::Geometry(const char *filepath)
{
	parse(filepath);
	setup();
}


Geometry::~Geometry()
{
}

void Geometry::parse(const char *file1)
{
	FILE* fp;
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
	unsigned int f1, f3, f5;
	char s[1000];
	fp = fopen(file1, "rb");
	if (fp == NULL) {
		std::cerr << "error reading file" << std::endl;
		exit(1);
	}
	while (!feof(fp)) {
		fgets(s, 1000, fp);
		if (s[0] == 'v') {
			if (s[1] == 'n') {
				sscanf(s, "vn %f %f %f", &nx, &ny, &nz);
				glm::vec3 vn = glm::vec3(nx, ny, nz);
				normals.push_back(vn);
			}
			else if (s[1] == ' ') {
				sscanf(s, "v %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				glm::vec3 v = glm::vec3(x, y, z);
				glm::vec3 c = glm::vec3(r, g, b);
				vertices.push_back(v);
				colors.push_back(c);
			}
		}
		else if (s[0] == 'f') {
			sscanf(s, "f %u//%u %u//%u %u//%u", &f1, &f1, &f3, &f3, &f5, &f5);
			indices.push_back(f1 - 1);
			indices.push_back(f3 - 1);
			indices.push_back(f5 - 1);
		}
	}
	printf("Obj sizes are %zd %zd\n", vertices.size(), indices.size());
}

void Geometry::setup() {
	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &color);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, color);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()* sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Geometry::update()
{
}

void Geometry::draw(int program, glm::mat4 M) {
	uProjection = glGetUniformLocation(program, "projection");
	uModelview = glGetUniformLocation(program, "modelview");
	modelview = Window::V * M;
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}