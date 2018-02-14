#version 450 core
// This is a sample fragment shader.

uniform mat4 model;

in vec3 fragNormal;
in vec3 fragVertice;
in vec3 light_color_out;

out vec4 color;

void main()
{
	// calculate normal in world coordinates
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	// calculate the location of this pixel in world coordinates
	vec3 fragPosition = vec3(model * vec4(fragVertice,1));

	color = vec4(light_color_out, 1.0f);
}

