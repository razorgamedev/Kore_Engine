#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

#include "shaders.h"

class Skybox {
public:
    void Load(std::vector<const char *> faces);
    void Load(const char * path);
    void Render(glm::mat4 view, glm::mat4 proj);
    
    GLuint Get_ID();
private:
    GLuint id;

    GLuint vao, vbo;

    Shader shader;
};

#endif // __SKYBOX_H__