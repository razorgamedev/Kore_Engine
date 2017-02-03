#ifndef OBJ_LOADER_H_INCLUDED
#define OBJ_LOADER_H_INCLUDED

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "mesh.h"

namespace obj{
    Mesh * Load_Mesh(std::string path);
}

struct Obj_Index
{
    unsigned int vertexIndex;
    unsigned int uvIndex;
    unsigned int normalIndex;
    
    bool operator<(const Obj_Index& r) const { return vertexIndex < r.vertexIndex; }
};

class Indexed_Model
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    
    void CalcNormals();
};

class Obj_Model
{
public:
    std::vector<Obj_Index> OBJIndices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool hasUVs;
    bool hasNormals;
    
    Obj_Model(const std::string& fileName);
    
    Indexed_Model To_Indexed_Model();
	//dust::raw_model To_Raw_Model(dust::loader & loader);
private:
    unsigned int find_last_vertex_index(const std::vector<Obj_Index*>& indexLookup, const Obj_Index* currentIndex, const Indexed_Model& result);
    void create_obj_face(const std::string& line);
    
    glm::vec2 parse_obj_vec2(const std::string& line);
    glm::vec3 parse_obj_vec3(const std::string& line);
    Obj_Index parse_obj_index(const std::string& token, bool* hasUVs, bool* hasNormals);
};

#endif // OBJ_LOADER_H_INCLUDED
