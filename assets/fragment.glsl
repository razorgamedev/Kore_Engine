#version 300 es

precision highp float;

in vec2 pass_uvs;
in vec3 surface_normal;
in vec3 to_light_vector[4];

in vec3 to_camera_vector;

in float visibility;

// in float out_number_of_lights;

out vec4 out_color;

uniform sampler2D texture_sampler;
uniform vec3 light_color[4];
uniform vec3 attenuation[4];
uniform vec3 sky_color;

uniform float shine_damper;
uniform float reflectivity;

const float Darkness = 0.2;

void main(void){
	vec3 unit_vector_to_camera  = normalize(to_camera_vector);
	vec3 unit_normal			= normalize(surface_normal);

	vec3 total_diffuse = vec3(0.0f);
	vec3 total_specular = vec3(0.0f);

	for (int i = 0; i < 4; i++){
		float distance = length(to_light_vector[i]);
		float att_factor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		vec3 unit_light_vector = normalize(to_light_vector[i]);
		
		float n_dot_1 		= dot(unit_normal, unit_light_vector);
		//float brightness 	= max(n_dot_1, 0.0f);
		float brightness 	= clamp(n_dot_1, 0.0f, 1.0f);
		
		vec3 light_direction 		= -unit_light_vector;
		vec3 reflected_light_dir 	= reflect(light_direction, unit_normal);
		
		float specular = dot(reflected_light_dir, unit_vector_to_camera);
		
		specular = max(specular, 0.0f);
		float damped_factor = pow(specular, shine_damper);
		
		total_diffuse  = total_diffuse  + (brightness 	* light_color[i]) / att_factor;
		total_specular = total_specular + (damped_factor * reflectivity * light_color[i]) / att_factor;
	}

	total_diffuse = max(total_diffuse, 0.2f);
	
	vec4 texture_color = texture(texture_sampler, pass_uvs);
	if (texture_color.a < 0.5){
		discard;
	}

	out_color = vec4(total_diffuse, 1.0) * texture_color + vec4(total_specular, 1.0);
	out_color = mix(vec4(sky_color, 1.0), out_color, visibility);
}

