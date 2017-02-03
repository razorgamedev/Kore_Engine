#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <map>
#include <vector>
#include <string>
#include "static_shader.h"
#include "model.h"
#include "color.h"
#include "fps_camera.h"
#include "lights.h"
#include "frustum.h"

class Camera;

class Renderer {
public:
	Renderer(int w, int h);
	
	void Clear();
	void Begin_3D();
	void End_3D(Camera & camera);
	void Direct_Render(Model &model);
	void Render(Model &m);
	void Final_Render(Camera & camera);

	void Load_All_Shaders();
	void Set_View_Matrix(glm::mat4 other);
	void Set_Clear_Color(Color _clear_color);

	void Add_Light(Light_3D &light);

	std::vector<Light_3D> & Get_Lights();
	void Set_Lights(std::vector<Light_3D> & _lights);

	void Toggle_Debug();

	glm::mat4 Get_Projection_Matrix();
private:
	void set_2d();
	void set_3d();

	Color clear_color;
	Static_Shader static_shader;
	glm::mat4 projection_matrix;

	glm::mat4 view_matrix;

	std::vector<Light_3D> lights;

	std::map<std::string, std::vector<Model>> render_group;

	bool debug;
};

#endif // __RENDERER_H__
