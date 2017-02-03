#ifndef __MESH_H__
#define __MESH_H__

#include <string>
#include <GL/glew.h>
#include <vector>

struct Bounding_Box{
	glm::vec3 origin;
	glm::vec3 half_size;
};

struct Mesh {
    std::string path;
    
    std::vector<GLfloat>    vertices;
    std::vector<GLfloat>    normals;
    std::vector<GLfloat>    uvs;
    std::vector<GLuint>     indices;

    Bounding_Box box;
};

#endif //__MESH_H__