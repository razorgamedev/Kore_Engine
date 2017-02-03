#include "scene_3d.h"

Scene_3D::~Scene_3D(){

}

Mesh * Scene_3D::Add_Mesh(Mesh * mesh){
    meshes.insert(std::make_pair(mesh->path, mesh));
    return mesh;
}

Texture Scene_3D::Add_Texture(Texture texture){
    textures.insert(std::make_pair(texture.Path, texture));
    return texture;
}

std::vector<Model> Scene_3D::Get_Unique_Model_List(){
    std::vector<Model> models;

    for (auto const & key : meshes){
        for (auto & o : meshes_and_textures)
            if (o.mesh_path == key.first){

                Model model;
                model.Load(
                    meshes[o.mesh_path],
                    textures[o.texture_path]
                );
                models.push_back(model);

                break;
            }
    }
    return models;
}

void Scene_3D::Load_Scene(const char * path, std::vector<Model> & models, std::vector<Light_3D> & lights){
    std::ifstream file(path);
    std::string line;

    models.clear();
    lights.clear();

    while(std::getline(file, line)){
        std::vector<std::string> tokens;

        std::stringstream ss(line);
        std::string temp;
        while(ss){
            ss >> temp;
            tokens.push_back(temp);
        }

        if (tokens[0] == "m"){
            auto mpath = tokens[1].c_str();
            auto tpath = tokens[9].c_str();

            mesh_and_texture_path map {
                mpath, tpath
            };

            meshes_and_textures.push_back(map);
        }
    }
    file.close();

    // load meshes
    load_all_meshes_and_textures(meshes_and_textures);
    // 

    file.open(path);

    while(std::getline(file, line)){
        std::vector<std::string> tokens;

        std::stringstream ss(line);
        std::string temp;
        while(ss){
            ss >> temp;
            tokens.push_back(temp);
        }
        Model model;

        if (tokens[0] == "m"){
            if (meshes.find(tokens[1]) != meshes.end()){
                auto mesh = meshes[tokens[1]];
                
                if (textures.find(tokens[9]) != textures.end()){
                    model.Load(mesh, 
                        textures.find(tokens[9])->second
                    );

                    model.Position.x = std::stof(tokens[1+1]);
                    model.Position.y = std::stof(tokens[2+1]);
                    model.Position.z = std::stof(tokens[3+1]);
                    model.Rotation.x = std::stof(tokens[4+1]);
                    model.Rotation.y = std::stof(tokens[5+1]);
                    model.Rotation.z = std::stof(tokens[6+1]);

                    Bounding_Box box;
                    box.origin = model.Position;
                    box.half_size = glm::vec3(2,2,2);
                    model.Set_Bounding_Box(box);

                    models.push_back(model);
                
                } else {
                    std::cout << "Cannot find texture: " << tokens[9] << std::endl;
                }
            }else{
                std::cout << "Cannot find mesh: " << tokens[1] << std::endl;
            }
        } else if (tokens[0] == "l"){
            Light_3D light;

            int n = 1;
            light.Position.x        = std::stof(tokens[n++]);
            light.Position.y        = std::stof(tokens[n++]);
            light.Position.z        = std::stof(tokens[n++]);
            light.Color.x           = std::stof(tokens[n++]);
            light.Color.y           = std::stof(tokens[n++]);
            light.Color.z           = std::stof(tokens[n++]);
            light.Attenuation.x     = std::stof(tokens[n++]);
            light.Attenuation.y     = std::stof(tokens[n++]);
            light.Attenuation.z     = std::stof(tokens[n++]);

            lights.push_back(light);
        }
    }
}

void Scene_3D::Export_Scene(const char * path, std::vector<Model> &models, std::vector<Light_3D> & lights){
    std::string _export = "";

    for (Model model : models){

        auto path = model.Get_Mesh()->path;

        std::string x = std::to_string(model.Position.x);
        std::string y = std::to_string(model.Position.y);
        std::string z = std::to_string(model.Position.z);

        std::string rx = std::to_string(model.Rotation.x);
        std::string ry = std::to_string(model.Rotation.y);
        std::string rz = std::to_string(model.Rotation.z);

        std::string s = std::to_string(
            (model.Scale.x + model.Scale.y + model.Scale.z) / 3.f 
        );

        std::string t_path = model.Get_Texture().Path;

        _export += "m "+ path + " " +
                   x + " " + y + " " + z + " " +
                   rx + " " + ry + " " + rz + " " + 
                   s + " " + t_path + "\n"; 
    }

    for (Light_3D light : lights){
        std::string x = std::to_string(light.Position.x);
        std::string y = std::to_string(light.Position.y);
        std::string z = std::to_string(light.Position.z);

        std::string r = std::to_string(light.Color.x);
        std::string g = std::to_string(light.Color.y);
        std::string b = std::to_string(light.Color.z);

        std::string ax = std::to_string(light.Attenuation.x);
        std::string ay = std::to_string(light.Attenuation.y);
        std::string az = std::to_string(light.Attenuation.z);

        _export += "l " + x + " " + y + " " + z + " " + r + " " + g + " " + b + " " + ax + " " + ay + " " + az + "\n";  
    }

    std::ofstream out(path);
    out << _export;
    out.close();
}

void Scene_3D::load_all_meshes_and_textures(std::vector<mesh_and_texture_path> & map){
    start_time();

    std::vector<std::future<Mesh * >> mesh_futures;
    std::vector<std::string> added_future_ids;

    for (auto m : map){
        if (textures.find(m.texture_path) == textures.end())
            Add_Texture(asset_loader::Load_Texture(m.texture_path.c_str(), GL_LINEAR, true));   
        
        bool add_mesh = true;
        for (auto s : added_future_ids){
            if (s == m.mesh_path) add_mesh = false;
        }

        if (add_mesh){    
            mesh_futures.push_back(std::async(std::launch::async, obj::Load_Mesh, m.mesh_path));
            added_future_ids.push_back(m.mesh_path);
        }
        
    }

    for (auto & f : mesh_futures){
         Add_Mesh(f.get());
    }

    stop_time("Scene loading time: ");
}