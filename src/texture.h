#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glew.h>
#include <iostream>
#include <string>

struct Texture {
    GLuint ID;
    std::string Path;

    int Width, Height;

    float Shine;
    float Reflectivity;
};

#endif // __TEXTURE_H__