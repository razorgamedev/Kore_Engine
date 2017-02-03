#include "static_shader.h"

Static_Shader::Static_Shader(){
}

void Static_Shader::Load() {
	Load_Shaders("assets/vertex.glsl", "assets/fragment.glsl");
	//bind_attributes();
}
void Static_Shader::bind_attributes() {
	bind_attribute(0, "vertices");
	bind_attribute(1, "uvs");
	bind_attribute(2, "normals");
}
