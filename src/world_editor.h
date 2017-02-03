#ifndef __GUI_RENDERER_H__
#define __GUI_RENDERER_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "model.h"
#include "gui_elements.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "fps_camera.h"
#include "timer.h"
#include "renderer.h"
#include "scene_3d.h"
#include "lights.h"

class World_Editor{
public:
    World_Editor(GLFWwindow * _window, std::vector<Model> & models);
    ~World_Editor();

    void Render_GUI(GLFWwindow * window,Renderer &renderer, std::vector<Model> & models, std::vector<Light_3D> & lights,Scene_3D & scene);
    void Update(Timer &timer);

    glm::mat4 Get_View_Matrix();
    Camera & Get_Camera();

    void Handle_Scroll_Wheel(float yscroll);
private:
    GLFWwindow * window;

    Camera camera;

    std::vector<Model> place_models;
    Model place_model;
    
    bool placing;
    int current_left_state, last_left_state;
    float fps;

    float depth;
};

#endif //__GUI_RENDERER_H__