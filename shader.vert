#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;
uniform vec3 viewPos;

out vec3 fragNormal;
out vec3 world_pos;
out float edge;
void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
	fragNormal =  normalize(normal);
	world_pos = vec3(mat4(model) * vec4(position,1.0f));

	edge = max(0, dot(fragNormal,normalize(viewPos-world_pos)));
}