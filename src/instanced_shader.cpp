#include "instanced_shader.h"

Instanced_Shader::Instanced_Shader(){

}

void Instanced_Shader::Load(){
    Load_Shaders("assets/instance_vertex.glsl", "assets/instance_fragment.glsl");
}

void Instanced_Shader::bind_attributes(){
    bind_attribute(0, "vertices");
}