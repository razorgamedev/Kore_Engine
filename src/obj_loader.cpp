#include "obj_loader.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

static bool CompareObj_IndexPtr(const Obj_Index* a, const Obj_Index* b);
static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
static inline unsigned int parse_obj_indexValue(const std::string& token, unsigned int start, unsigned int end);
static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
static inline std::vector<std::string> SplitString(const std::string &s, char delim);

Obj_Model::Obj_Model(const std::string& fileName)
{
	hasUVs = false;
	hasNormals = false;
    std::ifstream file;
    file.open(fileName.c_str());

    std::string line;
    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
        
            unsigned int lineLength = line.length();
            
            if(lineLength < 2)
                continue;
            
            const char* lineCStr = line.c_str();
            
            switch(lineCStr[0])
            {
                case 'v':
                    if(lineCStr[1] == 't')
                        this->uvs.push_back(parse_obj_vec2(line));
                    else if(lineCStr[1] == 'n')
                        this->normals.push_back(parse_obj_vec3(line));
                    else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
                        this->vertices.push_back(parse_obj_vec3(line));
                break;
                case 'f':
                    create_obj_face(line);
                break;
                default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << fileName << std::endl;
    }
}

void Indexed_Model::CalcNormals()
{
    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];

        glm::vec3 v1 = positions[i1] - positions[i0];
        glm::vec3 v2 = positions[i2] - positions[i0];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
            
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    
    for(unsigned int i = 0; i < positions.size(); i++)
        normals[i] = glm::normalize(normals[i]);
}

Indexed_Model Obj_Model::To_Indexed_Model()
{
    Indexed_Model result;
    Indexed_Model normalModel;
    
    unsigned int numIndices = OBJIndices.size();
    
    std::vector<Obj_Index*> indexLookup;
    
    for(unsigned int i = 0; i < numIndices; i++)
        indexLookup.push_back(&OBJIndices[i]);
    
    std::sort(indexLookup.begin(), indexLookup.end(), CompareObj_IndexPtr);
    
    std::map<Obj_Index, unsigned int> normalModelIndexMap;
    std::map<unsigned int, unsigned int> indexMap;
    
    for(unsigned int i = 0; i < numIndices; i++)
    {
        Obj_Index* currentIndex = &OBJIndices[i];
        
        glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
        glm::vec2 currentTexCoord;
        glm::vec3 currentNormal;
        
        if(hasUVs)
            currentTexCoord = uvs[currentIndex->uvIndex];
        else
            currentTexCoord = glm::vec2(0,0);
            
        if(hasNormals)
            currentNormal = normals[currentIndex->normalIndex];
        else
            currentNormal = glm::vec3(0,0,0);
        
        unsigned int normalModelIndex;
        unsigned int resultModelIndex;
        
        //Create model to properly generate normals on
        std::map<Obj_Index, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
        if(it == normalModelIndexMap.end())
        {
            normalModelIndex = normalModel.positions.size();
        
            normalModelIndexMap.insert(std::pair<Obj_Index, unsigned int>(*currentIndex, normalModelIndex));
            normalModel.positions.push_back(currentPosition);
            normalModel.texCoords.push_back(currentTexCoord);
            normalModel.normals.push_back(currentNormal);
        }
        else
            normalModelIndex = it->second;
        
        //Create model which properly separates texture coordinates
        unsigned int previousVertexLocation = find_last_vertex_index(indexLookup, currentIndex, result);
        
        if(previousVertexLocation == (unsigned int)-1)
        {
            resultModelIndex = result.positions.size();
        
            result.positions.push_back(currentPosition);
            result.texCoords.push_back(currentTexCoord);
            result.normals.push_back(currentNormal);
        }
        else
            resultModelIndex = previousVertexLocation;
        
        normalModel.indices.push_back(normalModelIndex);
        result.indices.push_back(resultModelIndex);
        indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
    }
    
    if(!hasNormals)
    {
        normalModel.CalcNormals();
        
        for(unsigned int i = 0; i < result.positions.size(); i++)
            result.normals[i] = normalModel.normals[indexMap[i]];
    }
    
    return result;
};

unsigned int Obj_Model::find_last_vertex_index(const std::vector<Obj_Index*>& indexLookup, const Obj_Index* currentIndex, const Indexed_Model& result)
{
    unsigned int start = 0;
    unsigned int end = indexLookup.size();
    unsigned int current = (end - start) / 2 + start;
    unsigned int previous = start;
    
    while(current != previous)
    {
        Obj_Index* testIndex = indexLookup[current];
        
        if(testIndex->vertexIndex == currentIndex->vertexIndex)
        {
            unsigned int countStart = current;
        
            for(unsigned int i = 0; i < current; i++)
            {
                Obj_Index* possibleIndex = indexLookup[current - i];
                
                if(possibleIndex == currentIndex)
                    continue;
                    
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                    
                countStart--;
            }
            
            for(unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
            {
                Obj_Index* possibleIndex = indexLookup[current + i];
                
                if(possibleIndex == currentIndex)
                    continue;
                    
                if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
                    break;
                else if((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex) 
                    && (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex))
                {
                    glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
                    glm::vec2 currentTexCoord;
                    glm::vec3 currentNormal;
                    
                    if(hasUVs)
                        currentTexCoord = uvs[currentIndex->uvIndex];
                    else
                        currentTexCoord = glm::vec2(0,0);
                        
                    if(hasNormals)
                        currentNormal = normals[currentIndex->normalIndex];
                    else
                        currentNormal = glm::vec3(0,0,0);
                    
                    for(unsigned int j = 0; j < result.positions.size(); j++)
                    {
                        if(currentPosition == result.positions[j] 
                            && ((!hasUVs || currentTexCoord == result.texCoords[j])
                            && (!hasNormals || currentNormal == result.normals[j])))
                        {
                            return j;
                        }
                    }
                }
            }
        
            return -1;
        }
        else
        {
            if(testIndex->vertexIndex < currentIndex->vertexIndex)
                start = current;
            else
                end = current;
        }
    
        previous = current;
        current = (end - start) / 2 + start;
    }
    
    return -1;
}

void Obj_Model::create_obj_face(const std::string& line)
{
    std::vector<std::string> tokens = SplitString(line, ' ');

    this->OBJIndices.push_back(parse_obj_index(tokens[1], &this->hasUVs, &this->hasNormals));
    this->OBJIndices.push_back(parse_obj_index(tokens[2], &this->hasUVs, &this->hasNormals));
    this->OBJIndices.push_back(parse_obj_index(tokens[3], &this->hasUVs, &this->hasNormals));

    if((int)tokens.size() > 4)
    {
        this->OBJIndices.push_back(parse_obj_index(tokens[1], &this->hasUVs, &this->hasNormals));
        this->OBJIndices.push_back(parse_obj_index(tokens[3], &this->hasUVs, &this->hasNormals));
        this->OBJIndices.push_back(parse_obj_index(tokens[4], &this->hasUVs, &this->hasNormals));
    }
}

Obj_Index Obj_Model::parse_obj_index(const std::string& token, bool* hasUVs, bool* hasNormals)
{
    unsigned int tokenLength = token.length();
    const char* tokenString = token.c_str();
    
    unsigned int vertIndexStart = 0;
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    Obj_Index result;
    result.vertexIndex = parse_obj_indexValue(token, vertIndexStart, vertIndexEnd);
    result.uvIndex = 0;
    result.normalIndex = 0;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    result.uvIndex = parse_obj_indexValue(token, vertIndexStart, vertIndexEnd);
    *hasUVs = true;
    
    if(vertIndexEnd >= tokenLength)
        return result;
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');
    
    result.normalIndex = parse_obj_indexValue(token, vertIndexStart, vertIndexEnd);
    *hasNormals = true;
    
    return result;
}

glm::vec3 Obj_Model::parse_obj_vec3(const std::string& line) 
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 2;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec3(x,y,z);

    //glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
}

glm::vec2 Obj_Model::parse_obj_vec2(const std::string& line)
{
    unsigned int tokenLength = line.length();
    const char* tokenString = line.c_str();
    
    unsigned int vertIndexStart = 3;
    
    while(vertIndexStart < tokenLength)
    {
        if(tokenString[vertIndexStart] != ' ')
            break;
        vertIndexStart++;
    }
    
    unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    vertIndexStart = vertIndexEnd + 1;
    vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');
    
    float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);
    
    return glm::vec2(x,y);
}

static bool CompareObj_IndexPtr(const Obj_Index* a, const Obj_Index* b)
{
    return a->vertexIndex < b->vertexIndex;
}

static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
{
    unsigned int result = start;
    while(result < length)
    {
        result++;
        if(str[result] == token)
            break;
    }
    
    return result;
}

static inline unsigned int parse_obj_indexValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atoi(token.substr(start, end - start).c_str()) - 1;
}

static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
{
    return atof(token.substr(start, end - start).c_str());
}

static inline std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::vector<std::string> elems;
        
    const char* cstr = s.c_str();
    unsigned int strLength = s.length();
    unsigned int start = 0;
    unsigned int end = 0;
        
    while(end <= strLength)
    {
        while(end <= strLength)
        {
            if(cstr[end] == delim)
                break;
            end++;
        }
            
        elems.push_back(s.substr(start, end - start));
        start = end + 1;
        end = start;
    }
        
    return elems;
}

Mesh * obj::Load_Mesh(std::string path){
    std::cout << "Loading mesh: " << path << std::endl;

    Mesh * mesh = new Mesh();
    mesh->path = path;

    auto obj_model = Obj_Model(path);
    auto indexed_model = obj_model.To_Indexed_Model();

    float x = 0, y = 0, z = 0;
    float x2 = 0, y2 = 0;

    for (auto v : indexed_model.positions){
        mesh->vertices.push_back(v.x);
        mesh->vertices.push_back(v.y);
        mesh->vertices.push_back(v.z);

        if (v.x < x) x = v.x;
        if (v.z < z) z = v.z; 
        if (v.y > y) y = v.y;
    }

    mesh->box.origin    = glm::vec3(x, y, z);
    mesh->box.half_size = glm::vec3(10, 10, 10);

    for (auto i : indexed_model.indices){
        mesh->indices.push_back(i);
    }

    for (auto uv : indexed_model.texCoords) {
        mesh->uvs.push_back(uv.x);
        mesh->uvs.push_back(uv.y);
    }

    for (auto norm : indexed_model.normals) {
        mesh->normals.push_back(norm.x);
        mesh->normals.push_back(norm.y);
        mesh->normals.push_back(norm.z);
    }

    return mesh;   
}