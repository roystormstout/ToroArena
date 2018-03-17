#version 330 core

in vec2 position;
in vec2 texcoord;

out vec2 vertTexCoord;

void main()
{
	vertTexCoord = texcoord;

    gl_Position = vec4(position,0.0,1.0);

}