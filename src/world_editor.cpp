#include "world_editor.h"

World_Editor::World_Editor(GLFWwindow * _window, std::vector<Model> & models)
{
    window = _window;

    for (auto model : models){
        place_models.push_back(model);
    }

    placing = false;
    current_left_state = last_left_state = GLFW_RELEASE;
    fps = 0;
    depth = 15;

    ImGui_ImplGlfw_Init(window, true);
}

World_Editor::~World_Editor(){
    ImGui_ImplGlfw_Shutdown();
}

Camera & World_Editor::Get_Camera(){
    return camera;
}

glm::mat4 World_Editor::Get_View_Matrix(){
    return camera.Get_View_Matrix();
}

void World_Editor::Handle_Scroll_Wheel(float yscroll){
    depth += yscroll;
}

void World_Editor::Update(Timer &timer){
    camera.Update(timer.dt * 3, window);

    fps = timer.fps;
}

void World_Editor::Render_GUI(GLFWwindow * window, Renderer &renderer, std::vector<Model> & models, std::vector<Light_3D> & lights,Scene_3D & scene){
    ImGui_ImplGlfw_NewFrame();

    ImGui::Begin("Data");
    auto fps_time = std::to_string(fps);
    ImGui::Text("FPS: ");
    ImGui::SameLine();
    ImGui::Text(fps_time.c_str());
    if (ImGui::Button("Debug")){
        renderer.Toggle_Debug();
    }
    ImGui::End();

    static char file_name[256] = "assets/test.scene";
    ImGui::Begin("File");

    ImGui::InputText("scene name", file_name, sizeof(char) * 256);
    if (ImGui::Button("Save")){
        scene.Export_Scene(file_name, models, lights);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")){
        scene.Load_Scene(file_name, models, lights);
    }

    ImGui::End();

    ImGui::Begin("Scene");
    ImGui::Text("-- Models --");
    {
        int _i = -1;
        bool end = false;
        for(Model & m : models){
            _i++;
            auto i_str      = std::to_string(_i);
            auto name       = i_str + ": " + m.Get_Mesh()->path;
            auto del        = "delete##" + i_str;
            auto child_str  = name + "##"+i_str;
            auto go_to      = "Go To##" + i_str;

            auto id_name = name + "##" + i_str;

            int at = name.rfind('/');
            name = name.substr(at+1);
            name = name.substr(0, name.size()-4);
            
            ImGui::Text(name.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth()-100);
            if (ImGui::Button(go_to.c_str()))camera.Position = m.Position + glm::vec3(0, 25, 0);
            ImGui::SameLine();

            auto edit = "Edit##"+i_str;
            if (ImGui::Button(edit.c_str())){
                ImGui::OpenPopup(id_name.c_str());
            }

            if (ImGui::BeginPopup(id_name.c_str())){
                end = true;
                if (ImGui::Button(del.c_str()))
                    models.erase(models.begin() + _i);

                std::string x = std::to_string(m.Position.x) +"##x"+ i_str, 
                            y = std::to_string(m.Position.y) +"##y"+ i_str, 
                            z = std::to_string(m.Position.z) +"##z"+ i_str;

                std::string rx = std::to_string(m.Rotation.x * 180/3.14159)+"##rx"+i_str,
                            ry = std::to_string(m.Rotation.y * 180/3.14159)+"##ry"+i_str,
                            rz = std::to_string(m.Rotation.z * 180/3.14159)+"##rz"+i_str;


                ImGui::Text("-- Position --");
                ImGui::InputFloat(x.c_str(), &m.Position.x, 0.5f, 0, 3);
                ImGui::InputFloat(y.c_str(), &m.Position.y, 0.5f, 0, 3);
                ImGui::InputFloat(z.c_str(), &m.Position.z, 0.5f, 0, 3);

                ImGui::Text("-- Rotation --");
                ImGui::InputFloat(rx.c_str(), &m.Rotation.x, 0.05f, 0, 3);
                ImGui::InputFloat(ry.c_str(), &m.Rotation.y, 0.05f, 0, 3);
                ImGui::InputFloat(rz.c_str(), &m.Rotation.z, 0.05f, 0, 3);
            }
            if (end){ ImGui::EndPopup(); end = !end;}

        }
    }
    ImGui::Text("-- Lights --");
    {
        int _i = 10;
        bool end = false;
        for (Light_3D & light : lights){
            _i++;
            std::string i_str       = std::to_string(-_i);
            std::string name        = "Light "  + i_str;
            std::string goto_str    = "Go To##" + i_str;
            std::string edit        = "Edit##"  + i_str;

            ImGui::BulletText(name.c_str());

            ImGui::SameLine(ImGui::GetWindowWidth()-100);
            if (ImGui::Button(goto_str.c_str())) camera.Position = light.Position;
            ImGui::SameLine();

            if (ImGui::Button(edit.c_str())){
                ImGui::OpenPopup(i_str.c_str());
            }

            if (ImGui::BeginPopup(i_str.c_str())){
                end = true;
                
                std::string x = std::to_string(light.Position.x) +"##lx"+ i_str, 
                            y = std::to_string(light.Position.y) +"##ly"+ i_str, 
                            z = std::to_string(light.Position.z) +"##lz"+ i_str;

                std::string r = std::to_string(light.Color.x * 180/3.14159)+"##lr"+i_str,
                            g = std::to_string(light.Color.y * 180/3.14159)+"##lg"+i_str,
                            b = std::to_string(light.Color.z * 180/3.14159)+"##lb"+i_str;

                std::string ax = std::to_string(light.Attenuation.x * 180/3.14159)+"##lax"+i_str,
                            ay = std::to_string(light.Attenuation.y * 180/3.14159)+"##lay"+i_str,
                            az = std::to_string(light.Attenuation.z * 180/3.14159)+"##laz"+i_str;


                ImGui::Text("-- Position --");
                ImGui::InputFloat(x.c_str(), &light.Position.x, 0.5f, 0, 3);
                ImGui::InputFloat(y.c_str(), &light.Position.y, 0.5f, 0, 3);
                ImGui::InputFloat(z.c_str(), &light.Position.z, 0.5f, 0, 3);

                ImGui::Text("-- Color --");
                ImGui::InputFloat(r.c_str(), &light.Color.x, 0.0005f, 0, 3);
                ImGui::InputFloat(g.c_str(), &light.Color.y, 0.0005f, 0, 3);
                ImGui::InputFloat(b.c_str(), &light.Color.z, 0.0005f, 0, 3);

                ImGui::Text("-- Attenuation --");
                ImGui::InputFloat(ax.c_str(), &light.Attenuation.x, 0.0005f, 0, 3);
                ImGui::InputFloat(ay.c_str(), &light.Attenuation.y, 0.0005f, 0, 3);
                ImGui::InputFloat(az.c_str(), &light.Attenuation.z, 0.0005f, 0, 3);
                
            }
            if (end){ ImGui::EndPopup(); end = !end;}

        }
    }

    ImGui::End(); // Scene

    ImGui::Begin("Meshes");

    int _i = -1;
    for (auto model : place_models){
        auto mesh = model.Get_Mesh();
        _i++;
        auto name = mesh->path;
        auto btn = name + "##"+std::to_string(_i);
        if (ImGui::Button(btn.c_str())){
            place_model = model;
            placing = true;
        }
    }

    ImGui::End();

    ImGui::Render();

    // render place model;
    if (placing){
        auto current_ray = camera.Calculate_Mouse_Ray(window, renderer.Get_Projection_Matrix());
        auto dist = depth;
        place_model.Position = camera.Position + (current_ray * dist);

        Bounding_Box box;
        box.half_size = glm::vec3(2,2,2);
        place_model.Set_Bounding_Box(box);
        place_model.Update_Bounding_Box();
        
        renderer.Render(place_model);

        current_left_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (current_left_state == GLFW_PRESS && last_left_state == GLFW_RELEASE){

            Model model;
            model.Load(place_model.Get_Mesh(), place_model.Get_Texture());
            model.Position = place_model.Position;
            model.Rotation = place_model.Rotation;

            Bounding_Box box;
            box.origin = model.Position;
            box.half_size = glm::vec3(2,2,2);
            model.Set_Bounding_Box(box);

            models.push_back(model);   

            placing = false;
        }
        last_left_state = current_left_state;
    }
}
