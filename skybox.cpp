#include "skybox.h"
#include "Window.h"
#define STB_IMAGE_IMPLEMENTATION
#include "packages//stb-master//stb_image.h"

float skyboxVertices[] = {
	// positions

	-1000.0f,  1000.0f, -1000.0f,
	-1000.0f, -1000.0f, -1000.0f,
	1000.0f, -1000.0f, -1000.0f,
	1000.0f, -1000.0f, -1000.0f,
	1000.0f,  1000.0f, -1000.0f,
	-1000.0f,  1000.0f, -1000.0f,

	-1000.0f, -1000.0f,  1000.0f,
	-1000.0f, -1000.0f, -1000.0f,
	-1000.0f,  1000.0f, -1000.0f,
	-1000.0f,  1000.0f, -1000.0f,
	-1000.0f,  1000.0f,  1000.0f,
	-1000.0f, -1000.0f,  1000.0f,

	1000.0f, -1000.0f, -1000.0f,
	1000.0f, -1000.0f,  1000.0f,
	1000.0f,  1000.0f,  1000.0f,
	1000.0f,  1000.0f,  1000.0f,
	1000.0f,  1000.0f, -1000.0f,
	1000.0f, -1000.0f, -1000.0f,

	-1000.0f, -1000.0f,  1000.0f,
	-1000.0f,  1000.0f,  1000.0f,
	1000.0f,  1000.0f,  1000.0f,
	1000.0f,  1000.0f,  1000.0f,
	1000.0f, -1000.0f,  1000.0f,
	-1000.0f, -1000.0f,  1000.0f,

	-1000.0f,  1000.0f, -1000.0f,
	1000.0f,  1000.0f, -1000.0f,
	-1000.0f,  1000.0f,  1000.0f,
	-1000.0f,  1000.0f,  1000.0f,
	1000.0f,  1000.0f,  -1000.0f,
	1000.0f,  1000.0f, 1000.0f,

	-1000.0f, -1000.0f, -1000.0f,
	-1000.0f, -1000.0f,  1000.0f,
	1000.0f, -1000.0f, -1000.0f,
	1000.0f, -1000.0f, -1000.0f,
	-1000.0f, -1000.0f,  1000.0f,
	1000.0f, -1000.0f,  1000.0f
};

std::vector<std::string> faces = {
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\right.ppm",
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\left.ppm",
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\up.ppm",
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\down.ppm",
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\back.ppm",
	"C:\\Users\\Lingfeng\\Desktop\\arrakisday\\front.ppm"
};

Skybox::Skybox()
{
	loadCubemap(faces);
}


Skybox::~Skybox()
{
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
}

void Skybox::loadCubemap(std::vector<std::string> faces){
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skybox::draw(GLuint skyboxShader) {
	glDepthMask(GL_FALSE);
	//glm::mat4 view = glm::mat4(glm::mat3(Window::V)); // remove translation from the view matrix
	uProjection = glGetUniformLocation(skyboxShader, "projection");
	uModelview = glGetUniformLocation(skyboxShader, "modelview");
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelview, 1, GL_FALSE, &Window::V[0][0]);
	glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE); // set depth function back to default
}