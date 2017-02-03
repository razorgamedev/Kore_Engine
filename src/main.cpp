#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <GLFW/glfw3.h>
#include <vector>
#include <thread>
#include <future>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "kaml_parser.h"
#include "obj_loader.h"
#include "renderer.h"
#include "color.h"
#include "fps_camera.h"
#include "timer.h"
#include "skybox.h"
#include "scene_3d.h"
#include "world_editor.h"
#include "window_utils.h"
#include "instanced_shader.h"

int main(){	
	// create the window
	auto window = create_window_and_init_opengl(800, 600, "Beyond Age");
	int w, h;
	glfwGetWindowSize(window, &w, &h);

	//create our modules
	Renderer 		renderer(w, h);
	Scene_3D 		scene;
	Timer 			timer;

	double load_timer = glfwGetTime();

	// testing 
	// auto obj = std::make_unique<kaml_object*>(parse_kaml_file("assets/data/test.kaml"));

	// return 0;
	// @refactor
	// we might want to move this into the scene graph
	start_time();
	Skybox skybox_1;
	{
		std::vector<const char *> faces;
		faces.push_back("assets/cubemap/right.png"	);
		faces.push_back("assets/cubemap/left.png"	);
		faces.push_back("assets/cubemap/top.png"	);
		faces.push_back("assets/cubemap/bottom.png"	);
		faces.push_back("assets/cubemap/back.png"	);
		faces.push_back("assets/cubemap/front.png"	);
		skybox_1.Load(faces);
	}
	stop_time("Skybox loading time: ");

	renderer.Load_All_Shaders();

	//@refactor, place the model/light vector in the scene graph?
	// current model scenes
	std::vector<Model> models;		// for world models
	std::vector<Light_3D> lights;	// for world lights
	scene.Load_Scene("assets/test.scene", models, lights);

	// give the renderer the lights
	renderer.Set_Lights(lights);
	
	// creates the place models for the world editor
	// @refactor
	auto place_models = scene.Get_Unique_Model_List();
	
	// create the world_editor
	World_Editor 	world_editor(window, place_models);

	// terrain.Scale.x = terrain.Scale.y = terrain.Scale.z = 8;
	// output the total load times, needs to be smaller
	std::cout << "\x1B[36m" << "Total load time: " << glfwGetTime() - load_timer << "\x1B[37m" << std::endl;

	// handle the scroll wheel, @refactor and move to the input class?
	static double yscroll = 0;
	auto scroll = [](GLFWwindow * window, double xoffset, double yoffset){
		yscroll = yoffset;
	};
	glfwSetScrollCallback(window, scroll);

	// main game loop
	while(glfwWindowShouldClose(window) == false){
		
		int w, h;
		glfwGetWindowSize(window, &w, &h);	// get the window width and height
		glViewport(0, 0, w, h);				// set the viewport 

		timer.Update();

		// send the mouse wheel to the editor @refactor
		if (yscroll != 0){
			world_editor.Handle_Scroll_Wheel((float)yscroll);
			yscroll = 0;
		}
		//update necessary modules
		world_editor.Update(timer);

		// clear and setup the 3d scene
		renderer.Clear();											// clears the pixel buffers and depth buffers
		renderer.Begin_3D();										// starts the 3d projection
		renderer.Set_Clear_Color(Color{0.8f, 0.8f, 0.8f, 1});		// sets the clear color to gray
		renderer.Set_View_Matrix(world_editor.Get_View_Matrix());	// give the renderer the view matrix

		// render all of the models in the scene, @refactor and just give the renderer
		// a reference to the vector?, or even the whole scene 
		for (auto _model : models){					
			renderer.Render(_model);
		}
		
		// actually render all of the models
		renderer.Final_Render(world_editor.Get_Camera());

		// draw the skybox in the world
		skybox_1.Render(world_editor.Get_View_Matrix(), renderer.Get_Projection_Matrix());
		
		// end the rendering process and allow other rendering techniques
		renderer.End_3D(world_editor.Get_Camera());

		// handle the ImGui and world editing things. @rename
		world_editor.Render_GUI(window, renderer, models, renderer.Get_Lights() ,scene);

		// glfw things
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}
