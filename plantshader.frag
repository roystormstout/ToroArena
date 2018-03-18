#version 330 core
// This is a sample fragment shader.

in vec3 normalcolor;
out vec4 color;

void main()
{
	color = vec4(normalcolor,1.0f);
}