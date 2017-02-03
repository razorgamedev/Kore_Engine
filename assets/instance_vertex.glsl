#version 300 es

in vec3 vertices;

out vec3 f_color;

uniform vec2 offsets[100];

void main(){
    vec2 offset = offsets[gl_InstanceID];
    vec3 final_pos = vertices + vec3(offset, 0);
    gl_Position = vec4(final_pos, 1.0f);

    f_color = vec3(vertices.x,vertices.y,0);
}