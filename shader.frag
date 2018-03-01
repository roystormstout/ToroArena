#version 330 core
// This is a sample fragment shader.


// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.


in vec3 normalcolor;


out vec4 color;


void main()
{

	color = vec4(normalcolor,1.0f);

}