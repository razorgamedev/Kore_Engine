#ifndef __LIGHTS_H__
#define __LIGHTS_H__

#include <glm/glm.hpp>

struct Light_3D{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Attenuation;

    Light_3D(){}

    Light_3D(glm::vec3 _position, glm::vec3 _color,glm::vec3 _attenuation = glm::vec3(1, 0, 0)){
        Position    = _position;
        Color       = _color;
        Attenuation = _attenuation;
    }
};

#endif // __LIGHTS_H__