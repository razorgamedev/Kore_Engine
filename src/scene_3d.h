#ifndef __SCENE_3D_H__
#define __SCENE_3D_H__

#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <thread>
#include <future>

#include "lights.h"
#include "model.h"
#include "mesh.h"
#include "texture.h"
#include "obj_loader.h"
#include "timer.h"
#include "asset_loader.h"

struct mesh_and_texture_path{
    std::string mesh_path;
    std::string texture_path;
};

struct Mesh_Future{
    std::future<Mesh *> mesh_future;
    std::string id;
};

class Scene_3D{
public:

    Mesh * Add_Mesh(Mesh * mesh);
    Texture Add_Texture(Texture texture);
    
    void Load_Scene(const char * path, std::vector<Model> & models, std::vector<Light_3D> & lights);
    void Export_Scene(const char * path,std::vector<Model> &models, std::vector<Light_3D> & lights);

    ~Scene_3D();

    std::vector<Model> & Get_Models();
    std::vector<Model> Get_Unique_Model_List();
private:
    std::map<std::string, Texture> textures;
    std::map<std::string, Mesh*> meshes;
    std::vector<Model> models; 

    std::vector<mesh_and_texture_path> meshes_and_textures;

    void load_all_meshes_and_textures(std::vector<mesh_and_texture_path> & map);
};

#endif //__SCENE_3D_H__