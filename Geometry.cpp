#pragma warning(disable : 4996)
#include "Geometry.h"



Geometry::Geometry(const char *filepath, glm::vec3 color, glm::vec3 move, int toon)
{
	usingtoon = toon;
	preset_color = color;
	toWorld = glm::translate(glm::mat4(1.0f), move);
	offset = { 0,0,0 };
	light_dir = {-20.0f, -40.0f, 30.0f };
	parse(filepath);
	box = new BoundingBox(vertices, move);
	setup();
}

Geometry::Geometry(const char *filepath, glm::vec3 color, float angle, glm::vec3 move, int toon)
{
	usingtoon = toon;
	preset_color = color;
	toWorld = glm::translate(glm::mat4(1.0f), move)* glm::rotate(glm::mat4(1.0f), 180.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));;
	light_dir = { 10.0f, -20.0f, 50.0f };
	parse(filepath);
	box = new BoundingBox(vertices, move);
	setup();
}

Geometry::~Geometry()
{
}

void Geometry::parse(const char *file1)
{
	FILE* fp;
	float x, y, z;
	float nx, ny, nz;
	unsigned int f1, f2, f3, f4, f5, f6;
	char s[1000];
	fp = fopen(file1, "rb");
	if (fp == NULL) {
		std::cerr << "error reading file" << std::endl;
		exit(1);
	}
	std::cerr << " reading file" << std::endl;
	while (!feof(fp)) {
		fgets(s, 1000, fp);
		if (s[0] == 'v') {
			if (s[1] == 'n') {
				sscanf(s, "vn %f %f %f", &nx, &ny, &nz);
				glm::vec3 vn = glm::vec3(nx, ny, nz);
				normals.push_back(vn);
			}
			else if (s[1] == ' ') {
				sscanf(s, "v %f %f %f", &x, &y, &z);
				glm::vec3 v = glm::vec3(x, y, z);
				vertices.push_back(v);
			}
		}
		else if (s[0] == 'f') {
			sscanf(s, "f %u//%u %u//%u %u//%u", &f1, &f2, &f3, &f4, &f5, &f6);
			indices.push_back(f1 - 1);
			indices.push_back(f3 - 1);
			indices.push_back(f5 - 1);
			indices2.push_back({ f1 - 1,f3 - 1,f5 - 1 , f2 - 1});
		}
	}
	calculate_normals();
	printf("Obj sizes are %zd %zd\n", vet_normals.size(), vertices.size());
	printf("%f %f %f\n", vet_normals[0].x, vet_normals[0].y, vet_normals[0].z);
	printf("%u %u %u\n", indices[0], indices[1], indices[2]);
	//for (int i = 0;i < normals.size();i++) {
	//	if (glm::dot(glm::normalize(normals[i]), Window::cam_pos) <= 0) {
	//		std::cout << "found!" << std::endl;
	//	}
	//}
}

void Geometry::calculate_normals() {
	for (int i = 0;i < vertices.size();i++) {
		glm::vec3 total = glm::vec3(0.0f);
		for (int j = 0;j < indices2.size();j++) {
			if (indices2[j].x == i || indices2[j].y == i || indices2[j].z == i) {
				total += normals[indices2[j].w];
			}
		}
		total = glm::normalize(total);
		vet_normals.push_back(total);
	}
}

void Geometry::setup() {
	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, vet_normals.size() * sizeof(glm::vec3), vet_normals.data(), GL_STATIC_DRAW);
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

void Geometry::draw(int program, bool tooning) {
	glUseProgram(program);
	uProjection = glGetUniformLocation(program, "projection");
	uModelview = glGetUniformLocation(program, "modelview");
	glUniform3f(glGetUniformLocation(program, "light_dir"), light_dir.x, light_dir.y, light_dir.z);
	glUniform3f(glGetUniformLocation(program, "presetColor"), preset_color.x, preset_color.y, preset_color.z);
	glUniform3f(glGetUniformLocation(program, "viewPos"), Window::cam_pos[0], Window::cam_pos[1], Window::cam_pos[2]);
	modelview = Window::V*toWorld;
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &toWorld[0][0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	if(tooning)
		glUniform1i(glGetUniformLocation(program, "toon"), usingtoon);
	else
		glUniform1i(glGetUniformLocation(program, "toon"), 0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::translate(glm::vec3 move) {
	offset += move;
	toWorld = glm::translate(glm::mat4(1.0f), move)*toWorld;
	box->update(move);
}

void Geometry::rotate() {
	toWorld = toWorld * glm::rotate(glm::mat4(1.0f), 1.0f / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}