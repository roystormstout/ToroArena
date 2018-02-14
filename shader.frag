#version 450 core
// This is a sample fragment shader.

struct DirLight{
vec3 light_color;
vec3 light_dir;
};

struct PointLight{
vec3 light_color;
vec3 light_pos;
float cons_att;
float linear_att;
float quad_att;
};

struct SpotLight{
vec3 light_color;
vec3 light_pos;
vec3 light_dir;

float cutoff_angle;
float outer_cutoff;
float exponent;
float cons_att;
float linear_att;
float quad_att;
};

uniform DirLight dir_light;
uniform SpotLight spot_light;
uniform PointLight point_light;

struct Material{
int object_mode;
vec3 color_diff;
vec3 color_spec;
vec3 color_ambi;
float p;
};

uniform mat4 model;
uniform int coloring_mode; 
uniform int dir_on; 
uniform int type; 
uniform vec3 viewPos;
uniform Material material;

in vec3 fragNormal;
in vec3 objNormal;
in vec3 fragPos; // fragVert

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{

	vec3 norm = normalize(fragNormal);

	vec3 viewDir = normalize(viewPos - fragPos);

	if (coloring_mode == 1) {
		color = vec4(objNormal, 1.0f); // normal coloring
	} else {
        vec3 result = vec3(0.0, 0.0, 0.0);

		if(dir_on == 1) {
			result +=  CalcDirLight(dir_light, norm, viewDir);
		}
		if (type == 1 ){
			color = vec4(result, 1.0);
		}

		else if (type == 2){
			result += CalcPointLight(point_light, norm, fragPos, viewDir);
			color = vec4(result, 1.0);
		}
		else if (type == 3){
			result += CalcSpotLight(spot_light, norm, fragPos, viewDir);
			color = vec4(result, 1.0);
		}

	}
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{		
	vec3 lightDir = normalize(-light.light_dir);
	// Ambient
	vec3 ambient = light.light_color * material.color_ambi; // ka * ca

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // (e * R)^p
	vec3 specular = light.light_color * (spec * material.color_spec); // material.color_spec * cl * (e * R) ^ p
       
        
	// Diffuse
	// the light's direction vector is the difference vector between light's position vector and fragment's position vector
	float diff = max(dot(normal, lightDir), 0.0); 
	vec3 diffuse = light.light_color * (diff * material.color_diff); // cl * (n*L)

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.light_pos - fragPos);

	//diffuse
	float diff =  max(dot(lightDir,normal),0.0);
	vec3 diffuse = light.light_color * material.color_diff * diff;

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular =  light.light_color * (spec * material.color_spec);

	// Ambient
	vec3 ambient = light.light_color * material.color_ambi; // ka * ca

	float distance = length(light.light_pos - fragPos);
	float attenuation = 1.0f / (light.cons_att + light.linear_att * distance);   
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;  
	return (diffuse+specular+ambient);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 lightDir = normalize(light.light_pos - fragPos);

	float theta = dot(normalize(light.light_dir),-lightDir);

	vec3 ambient =  material.color_ambi*light.light_color;

	vec3 color = light.light_color * pow(theta,light.exponent);

	if(theta > light.cutoff_angle)
	{
		vec3 diffuse = color * material.color_diff * max(dot(normal, lightDir),0.0);

		vec3 reflectDir = reflect(-lightDir, normal);
		vec3 specular = color * material.color_spec * pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);

		float distance = length(light.light_pos - fragPos);
		float attenuation =  1.0 / (light.cons_att + light.linear_att * distance + light.quad_att * distance * distance);

		diffuse = diffuse * attenuation;
		specular = specular * attenuation;

		return (diffuse + specular + ambient);
	}
	else
		return ambient;
}