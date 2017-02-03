#version 300 es

in vec3 vertices;
in vec2 uvs;
in vec3 normals;

out vec2 pass_uvs;
out vec3 surface_normal;
out vec3 to_light_vector[4];
out vec3 to_camera_vector;

out float visibility;
// out float out_number_of_lights;

uniform mat4 proj;
uniform mat4 trans;
uniform mat4 view;

// uniform float number_of_lights;
uniform vec3 light_position[4];

void main( void ){
	const float density = 0.003;
	const float gradient = 4.0f;
	
	vec4 world_position = trans * vec4(vertices, 1.0);

	gl_Position = proj * view * world_position;
	pass_uvs = uvs;

	surface_normal = (trans * vec4(normals, 0.0)).xyz;
	
	for (int i = 0; i < 4; i++){
		to_light_vector[i] = light_position[i] - world_position.xyz;
		// to_light_vector[i] = vec3(0, 100, 0) - world_position.xyz;
	}

	to_camera_vector = (inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz - world_position.xyz;

	// fog calc
	vec4 pos_relative_to_cam = view * world_position;
	float distance = length(pos_relative_to_cam.xyz);
	
	visibility = exp(-pow((distance * density),gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	// out_number_of_lights = number_of_lights;
}


