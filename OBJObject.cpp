#pragma warning(disable : 4996)
#include "OBJObject.h"
#include "Window.h"

Light dirLight(1);
Light pointLight(2);
Light spotLight(3);

OBJObject::OBJObject()
{
}

OBJObject::OBJObject(const char *file1, glm::vec3 s, glm::vec3 d, glm::vec3 a)
{

	toWorld = glm::mat4(1.0f);

	rotateMat = glm::mat4(1.0f);

	translateMat = glm::mat4(1.0f);

	scaleMat = glm::mat4(1.0f);

	glm::vec3 offset = { 0,0,0 };

	spec = s;
	diffuse = d;
	ambi = a;

	light_mode = 1;
	dir_light_on = 1;
	sptLightOut = 12.5f;
	sptLightIn = 9.5f;

	exponent = 10.0f;
	parse(file1);

	// Create array object and buffers. Remember to delete your buffers when the object is destroyed!
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO); // for normal coloring
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
	// Consider the VAO as a container for all your buffers.
	glBindVertexArray(VAO);

	// Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
	// you want to draw, such as vertices, normals, colors, etc.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
	// the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0 (check the vertex shader to see what this is)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

		// We've sent the vertex data over to OpenGL, but there's still something missing.
		// In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.

	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1); // Enable the usage of layout location 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // when layout=1

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind the VAO now so we don't accidentally tamper with it.
	// NOTE: You must NEVER unbind the element array buffer associated with a VAO!
	glBindVertexArray(0);

	parse_light("C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\cone.obj", "C:\\Users\\Lingfeng\\Documents\\CSE167StarterCode2-master\\sphere.obj");
	setModernOpenGL();
}

OBJObject::~OBJObject()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &NBO);
}

void normalize(std::vector<glm::vec3>& vertices) {
	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = 100000.0f;
	max_x = -100000.0f;
	min_y = 100000.0f;
	max_y = -100000.0f;
	min_z = 100000.0f;
	max_z = -100000.0f;
	for (int i = 0;i < vertices.size(); i++) {
		if (min_x > vertices[i].x)
			min_x = vertices[i].x;
		if (max_x < vertices[i].x)
			max_x = vertices[i].x;

		if (min_y > vertices[i].y)
			min_y = vertices[i].y;
		if (max_y < vertices[i].y)
			max_y = vertices[i].y;

		if (min_z > vertices[i].z)
			min_z = vertices[i].z;
		if (max_z < vertices[i].z)
			max_z = vertices[i].z;
	}
	float mid_x = (min_x + max_x) / 2;
	float mid_y = (min_y + max_y) / 2;
	float mid_z = (min_z + max_z) / 2;

	float range_x = max_x - min_x;
	float max = range_x;
	float range_y = max_y - min_y;
	float range_z = max_z - min_z;
	if (max < range_y)
		max = range_y;
	if (max < range_z)
		max = range_z;
	for (int i = 0;i < vertices.size(); i++) {
		vertices[i].x = (vertices[i].x - mid_x) * 2 / max;
		vertices[i].y = (vertices[i].y - mid_y) * 2 / max;
		vertices[i].z = (vertices[i].z - mid_z) * 2 / max;
	}
}

void OBJObject::parse(const char *file1)
{
	FILE* fp;
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
	unsigned int f1, f3,f5;
	char s[1000];
	fp = fopen(file1, "rb");
	if (fp == NULL) { 
		cerr << "error reading file" << endl; 
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
				glm::vec3 c = glm::vec3(r,g,b);
				vertices.push_back(v);
			}
		}
		else if (s[0] == 'f') {
			sscanf(s, "f %u//%u %u//%u %u//%u", &f1, &f1, &f3, &f3, &f5, &f5);
			indices.push_back(f1 - 1);
			indices.push_back(f3 - 1);
			indices.push_back(f5 - 1);
		}
	}
	normalize(vertices);
	printf("Obj sizes are %zd %zd\n", vertices.size(), indices.size());
}


void OBJObject::draw(GLuint shaderProgram)
{
	glm::mat4 modelview = Window::V * toWorld;
	glm::mat4 model = toWorld;
	//float distance = glm::length(pointLight.light_pos - glm::vec3(model *  glm::vec4(vertices[0].x, vertices[0].y, vertices[0].z, 1.0f)));
	//printf("%f \n", distance);
	// We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
	// Consequently, we need to forward the projection, view, and model matrices to the shader programs
	// Get the location of the uniform variables "projection" and "modelview"
	uProjection = glGetUniformLocation(shaderProgram, "projection");
	uModelview = glGetUniformLocation(shaderProgram, "modelview");
	// Now send these values to the shader program
	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 20.0f);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

	glUniform3f(glGetUniformLocation(shaderProgram, "material.color_spec"), spec.x, spec.y, spec.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.color_diff"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "material.color_ambi"), ambi.x, ambi.y, ambi.z);

	//Lighting
	glUniform1i(glGetUniformLocation(shaderProgram, "type"), light_mode);
	//Lighting
	glUniform1i(glGetUniformLocation(shaderProgram, "dir_on"), dir_light_on);
	//Dir 
	glUniform3f(glGetUniformLocation(shaderProgram, "dir_light.light_color"), dirLight.light_color.x, dirLight.light_color.y, dirLight.light_color.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "dir_light.light_dir"), dirLight.light_dir.x, dirLight.light_dir.y, dirLight.light_dir.z);
	//Point
	glUniform3f(glGetUniformLocation(shaderProgram, "point_light.light_color"), pointLight.light_color.x, pointLight.light_color.y, pointLight.light_color.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "point_light.light_pos"), pointLight.light_pos.x, pointLight.light_pos.y, pointLight.light_pos.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "point_light.cons_att"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "point_light.linear_att"), 0.07f);
	glUniform1f(glGetUniformLocation(shaderProgram, "point_light.quad_att"), 0.017f);
	//Spot
	glUniform3f(glGetUniformLocation(shaderProgram, "spot_light.light_color"), spotLight.light_color.x, spotLight.light_color.y, spotLight.light_color.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spot_light.light_dir"), spotLight.light_dir.x, spotLight.light_dir.y, spotLight.light_dir.z);
	glUniform3f(glGetUniformLocation(shaderProgram, "spot_light.light_pos"), spotLight.light_pos.x, spotLight.light_pos.y, spotLight.light_pos.z);
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.cutoff_angle"), glm::cos(glm::radians(sptLightOut)));
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.outer_cutoff"), glm::cos(glm::radians(sptLightIn)));
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.exponent"), exponent);
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.quad_att"), pointLight.quad_att);
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.cons_att"), 1.0f);
	glUniform1f(glGetUniformLocation(shaderProgram, "spot_light.linear_att"), 0.07f);

	// Now draw the cube. We simply need to bind the VAO associated with it.
	glBindVertexArray(VAO);
	// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
	glBindVertexArray(0);
}

void OBJObject::update()
{
	dirLight.update();
}

void OBJObject::reset()
{
}

void OBJObject::rotate_obj(float rot_angle, glm::vec3 rotAxis)
{
	toWorld = glm::translate(glm::mat4(1.0f), { -offset.x,-offset.y,-offset.z })* toWorld;
	toWorld = glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rotAxis) * toWorld;
	toWorld = glm::translate(glm::mat4(1.0f), offset)*toWorld;
}

void OBJObject::scale_obj(float scale)
{
	toWorld = toWorld * glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
}

void OBJObject::translate_obj(glm::vec3 direction) {
	offset += direction;
	toWorld = glm::translate(glm::mat4(1.0f), direction)*toWorld;
}

void OBJObject::rotate_light(float rot_angle, glm::vec3 rotAxis)
{
	if (light_mode == 2) {
		pointLight.light_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rotAxis) * glm::vec4(pointLight.light_pos, 1.0f));
		//pointLight.toWorld = translate(mat4(1.0f), vec3(pointLight.light_pos.x, pointLight.light_pos.y, pointLight.light_pos.z))*mat4(1.0f);
	}
	else if (light_mode == 3) {
		spotLight.light_pos = glm::vec3(glm::rotate(glm::mat4(1.0f), rot_angle / 180.0f * glm::pi<float>(), rotAxis) * glm::vec4(spotLight.light_pos, 1.0f));
		spotLight.light_dir = -spotLight.light_pos;
		//spotLight.toWorld = translate(mat4(1.0f), vec3(spotLight.light_pos.x, spotLight.light_pos.y, spotLight.light_pos.z))*mat4(1.0f);
	}
}

void OBJObject::draw_light(GLuint shaderProgram, int light_num) {
	if (light_mode == 2) {
		glm::mat4 model = translate(mat4(1.0f), vec3(pointLight.light_pos.x, pointLight.light_pos.y, pointLight.light_pos.z))*mat4(1.0f);
		glm::mat4 view = Window::V;
		uProjection = glGetUniformLocation(shaderProgram, "projection");
		uModel = glGetUniformLocation(shaderProgram, "model");
		uView = glGetUniformLocation(shaderProgram, "view");
		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
		glUniform3f(glGetUniformLocation(shaderProgram, "light_color"), 1.0f,1.0f,1.0f);
		glBindVertexArray(Light_VAO);
		// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
		printf("Sphere sizes are %zd\n", indices_p.size());
		glDrawElements(GL_TRIANGLES, indices_p.size(), GL_UNSIGNED_INT, 0);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
		glBindVertexArray(0);
		
	}
	else if (light_mode == 3){
		glm::mat4 rot = rotate(mat4(1.0f), glm::pi<float>(), vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 look = lookAt(spotLight.light_dir, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 model = translate(mat4(1.0f), vec3(spotLight.light_pos.x, spotLight.light_pos.y, spotLight.light_pos.z))*look*rot;
		glm::mat4 view = Window::V;
		uProjection = glGetUniformLocation(shaderProgram, "projection");
		uModel = glGetUniformLocation(shaderProgram, "model");
		uView = glGetUniformLocation(shaderProgram, "view");
		// Now send these values to the shader program
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
		glUniformMatrix4fv(uModel, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(uView, 1, GL_FALSE, &view[0][0]);
		glUniform3f(glGetUniformLocation(shaderProgram, "light_color"), 1.0f, 1.0f, 1.0f);
		glBindVertexArray(Light_VAO2);
		// Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
		printf("Cone sizes are %zd\n", indices_s.size());
		glDrawElements(GL_TRIANGLES, indices_s.size(), GL_UNSIGNED_INT, 0);
		// Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
		glBindVertexArray(0);
	}

}

void OBJObject::parse_light(const char* filepath, const char* filepath2)
{
	FILE* fp, *fp2;
	float x, y, z;
	float r, g, b;
	float nx, ny, nz;
	unsigned int f1, f3, f5;
	char s[1000];
	fp = fopen(filepath, "rb");
	if (fp == NULL) {
		cerr << "error reading file" << endl;
		exit(1);
	}
	while (!feof(fp)) {
		fgets(s, 1000, fp);
		if (s[0] == 'v') {
			if (s[1] == 'n') {
				sscanf(s, "vn %f %f %f", &nx, &ny, &nz);
				glm::vec3 vn = glm::vec3(nx, ny, nz);
				normals_s.push_back(vn);
			}
			else if (s[1] == ' ') {
				sscanf(s, "v %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				glm::vec3 v = glm::vec3(x/4.0f, y / 4.0f, z / 4.0f);
				glm::vec3 c = glm::vec3(r, g, b);
				vertices_s.push_back(v);
			}
		}
		else if (s[0] == 'f') {
			sscanf(s, "f %u//%u %u//%u %u//%u", &f1, &f1, &f3, &f3, &f5, &f5);
			indices_s.push_back(f1 - 1);
			indices_s.push_back(f3 - 1);
			indices_s.push_back(f5 - 1);
		}
	}
	printf("Cone sizes are %zd\n", indices_s.size());
	fp2 = fopen(filepath2, "rb");
	if (fp2 == NULL) {
		cerr << "error reading file" << endl;
		exit(1);
	}
	while (!feof(fp2)) {
		fgets(s, 1000, fp2);
		if (s[0] == 'v') {
			if (s[1] == 'n') {
				sscanf(s, "vn %f %f %f", &nx, &ny, &nz);
				glm::vec3 vn = glm::vec3(nx, ny, nz);
				normals_p.push_back(vn);
			}
			else if (s[1] == ' ') {
				sscanf(s, "v %f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
				glm::vec3 v = glm::vec3(x / 4.0f, y / 4.0f, z / 4.0f);
				glm::vec3 c = glm::vec3(r, g, b);
				vertices_p.push_back(v);
			}
		}
		else if (s[0] == 'f') {
			sscanf(s, "f %u//%u %u//%u %u//%u", &f1, &f1, &f3, &f3, &f5, &f5);
			indices_p.push_back(f1 - 1);
			indices_p.push_back(f3 - 1);
			indices_p.push_back(f5 - 1);
		}
	}
}

void OBJObject::setModernOpenGL()
{

	// Create array object and buffers.
	glGenVertexArrays(1, &Light_VAO);
	glGenBuffers(1, &Light_VBO_vertices);
	glGenBuffers(1, &Light_VBO_normals);
	glGenBuffers(1, &Light_EBO);

	// Bind the Vertex Array Object (VAO)
	glBindVertexArray(Light_VAO);

	// Bind VBO_vertices to it as a GL_ARRAY_BUFFER.
	glBindBuffer(GL_ARRAY_BUFFER, Light_VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices_p.size(), vertices_p.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind VBO_normals to it as a GL_ARRAY_BUFFER.
	glBindBuffer(GL_ARRAY_BUFFER, Light_VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals_p.size(), normals_p.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind the Element Buffer Object (EBO)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Light_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_p.size() * sizeof(GLuint), indices_p.data(), GL_STATIC_DRAW);

	// Unbind to avoid unexpected changes
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &Light_VAO2);
	glGenBuffers(1, &Light_VBO_vertices2);
	glGenBuffers(1, &Light_VBO_normals2);
	glGenBuffers(1, &Light_EBO2);

	// Bind the Vertex Array Object (VAO)
	glBindVertexArray(Light_VAO2);

	// Bind VBO_vertices to it as a GL_ARRAY_BUFFER.
	glBindBuffer(GL_ARRAY_BUFFER, Light_VBO_vertices2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices_s.size(), vertices_s.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind VBO_normals to it as a GL_ARRAY_BUFFER.
	glBindBuffer(GL_ARRAY_BUFFER, Light_VBO_normals2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals_s.size(), normals_s.data(), GL_STATIC_DRAW);
	// Enable the usage of layout location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Bind the Element Buffer Object (EBO)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Light_EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_s.size() * sizeof(GLuint), indices_s.data(), GL_STATIC_DRAW);

	// Unbind to avoid unexpected changes
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}