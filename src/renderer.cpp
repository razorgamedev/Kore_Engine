#include "renderer.h"

Renderer::Renderer(int w, int h)
{
	// init rendering stuff
	clear_color = Color{ 0.2f, 0.2f, 0.2f, 0.2f };
	view_matrix = glm::lookAt(
						glm::vec3(0, 0, 1),
						glm::vec3(0, 0, 0),
						glm::vec3(0, 1, 0)
					);

	// @fix this might be wrong, the FOV doesnt seem to work 
	projection_matrix = glm::perspective(45.f, ((float)w / (float)h), 0.001f, 1000.f);
}

void Renderer::Load_All_Shaders() {
	// load the shaders and compile them
	static_shader.Load();
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the buffers
	glClearColor(GETCOLOR(clear_color));				// set the clear color
}

void Renderer::Set_View_Matrix(glm::mat4 other) {
	view_matrix = other;
}

void Renderer::Set_Clear_Color(Color _clear_color) {
	this->clear_color = _clear_color;
}

void Renderer::Set_Lights(std::vector<Light_3D> & _lights){
	// @change? 
	for(auto l : _lights) lights.push_back(l); // puts the lights into the respected vector
}

std::vector<Light_3D> & Renderer::Get_Lights(){
	return lights;
}

glm::mat4 Renderer::Get_Projection_Matrix() { return projection_matrix; }

//@unused
constexpr int MAX_LIGHTS = 4;

void Renderer::Begin_3D() {

	// sets the opengl enables
	set_3d();

	// set the current shader to the static one, maybe we need this to be different to allow 
	// multiple materials?
	static_shader.Set();

	// set the respected uniforms to the matrices
	static_shader.Set_Uniform("proj", projection_matrix);
	static_shader.Set_Uniform("view", view_matrix);

	// give it the number of lights used, @unused
	static_shader.Set_Uniform("number_of_lights", (float)lights.size());

	// set the lighting uniforms and give them the respected values
	for (unsigned int i = 0; i < lights.size(); i++){
		auto light = lights[i];

		float pos[] = { light.Position.x, light.Position.y, light.Position.z };
		float col[] = { light.Color.x, light.Color.y, light.Color.z};
		float att[] = { light.Attenuation.x, light.Attenuation.y, light.Attenuation.z};

		std::string i_str = std::to_string(i);
		
		std::string light_pos = std::string{"light_position[" + i_str + "]"};
		static_shader.Set_Uniform(light_pos.c_str(), pos, 3);

		std::string light_color = std::string{"light_color[" + i_str + "]"}; 
		static_shader.Set_Uniform(light_color.c_str(), col, 3);

		std::string light_atten = std::string{"attenuation[" + i_str + "]"};
		static_shader.Set_Uniform(light_atten.c_str(), att, 3);
	}

	// dont think this is useful, we need to have the fog color be picked from the skybox
	float skyc [] = { clear_color.r, clear_color.g, clear_color.b };
	static_shader.Set_Uniform("sky_color", skyc, 4);
}

void Renderer::Toggle_Debug(){
	debug = !debug;
}

// this function doesnt use the texture binding improvement, but it just directly renders the models
// passed in, might need to change this
void Renderer::Direct_Render(Model & model) {
	// bind the vao containing the mesh data
	glBindVertexArray(model.Get_Vao());

	glEnableVertexAttribArray(0); // vertices
	glEnableVertexAttribArray(1); // uvs
	glEnableVertexAttribArray(2); // normals

	// get then bind the texture, while giving the shader the textures values
	auto t = model.Get_Texture();
	static_shader.Set_Uniform("shine_damper", t.Shine);
	static_shader.Set_Uniform("reflectivity", t.Reflectivity);

	glEnable(GL_TEXTURE_2D);			// this is dumb because i do it up above in set_3d?, @refactor
	glBindTexture(GL_TEXTURE_2D, t.ID); // bind the texture to the model
	glEnable(GL_TEXTURE0);				// tell opengl to use texture0

	// set the model transform in the shader
	auto transform = model.Get_Transform();
	static_shader.Set_Uniform("trans", transform);

	// draw the model using its index buffer
	glDrawElements(GL_TRIANGLES, model.Get_Mesh()->indices.size(), GL_UNSIGNED_INT, 0);

	// clean up and unbind the model
	glDisableVertexAttribArray(0); // vertices
	glDisableVertexAttribArray(1); // uvs
	glDisableVertexAttribArray(2); // normals

	glBindVertexArray(0);
}

void Renderer::Render(Model & m){
	// checks if the mesh currently excists in the render_goup map vector
	// if it doesnt then create the respected entry in the map 
	if (render_group.find(m.Get_Mesh()->path) == render_group.end()){
		render_group.insert(
			std::make_pair(m.Get_Mesh()->path, std::vector<Model>())
		);
	}

	// add the model to the create map index
	render_group[m.Get_Mesh()->path].push_back(m); 
}

// takes all of the models in the map and renders them
void Renderer::Final_Render(Camera & camera){
	// construct the model view matrix, this is not used in shading but in the view frustum
	// culling, also could be used for billboarding?
	const glm::mat4 mvp = projection_matrix * camera.Get_View_Matrix();
	Frustum frustum(mvp);	// construct a view frustum

	// loop through all of the indexes in the map
	for (auto v : render_group){
		auto id = v.first;			// extract the path
		auto models = v.second;     // extract the model vector

		// loop through all of the models in the vector
		unsigned int i = 0;
		for(Model model : models){
			if (i == 0){
				// if its the first model to be rendered, bind the mesh and textures, only do this once so
				// that we arent binding the same texture and mesh for every model, very effective!
				glBindVertexArray(model.Get_Vao());
				glEnableVertexAttribArray(0); // vertices
				glEnableVertexAttribArray(1); // uvs
				glEnableVertexAttribArray(2); // normals

				auto t = model.Get_Texture();
				static_shader.Set_Uniform("shine_damper", t.Shine);
				static_shader.Set_Uniform("reflectivity", t.Reflectivity);

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, t.ID);
				glEnable(GL_TEXTURE0);
			}

			// set the transform of the model in the shader
			auto transform = model.Get_Transform();
			static_shader.Set_Uniform("trans", transform);
			
			// get the models bounding box values, @broken
			const glm::vec3 origin 	= model.Get_Bounding_Box().origin;
			const glm::vec3 half 	= model.Get_Bounding_Box().half_size;
			
			// @fix the mesh bounding boxes then fix this!
			// this checks if the model is within the view frustum, if so render the model
			if (frustum.isBoxInFrustum(origin, half)) {
			}
				glDrawElements(GL_TRIANGLES, model.Get_Mesh()->indices.size(), GL_UNSIGNED_INT, 0);

			// if is the last model to be rendered, un bind the mesh pointers
			if (i == models.size()-1){
				//end;

				glDisableVertexAttribArray(0); // verptices
				glDisableVertexAttribArray(1); // uvs
				glDisableVertexAttribArray(2); // normals

				glBindVertexArray(0);
			}

			// increment the counter
			i++;
		}

		// debug rendering, @move @refactor
		if (debug){
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glLoadMatrixf(glm::value_ptr(projection_matrix));
		
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glLoadMatrixf(glm::value_ptr(camera.Get_View_Matrix()));

			glLineWidth(3);

			for(Model model : models){
				auto bound = model.Get_Bounding_Box();
				auto pos = model.Get_Bounding_Box_Origin();
				glTranslatef(pos.x, pos.y, pos.z);
				// printf("%f\n", bound.origin.x);

				glPushMatrix();
				glBegin(GL_LINE_LOOP);

				//front face
				glVertex3f(-bound.half_size.x, 	bound.half_size.y, -bound.half_size.z);
				glVertex3f(-bound.half_size.x, -bound.half_size.y, -bound.half_size.z);
				glVertex3f( bound.half_size.x, -bound.half_size.y, -bound.half_size.z);
				glVertex3f( bound.half_size.x,  bound.half_size.y, -bound.half_size.z);

				//back face
				glVertex3f(-bound.half_size.x, 	bound.half_size.y, bound.half_size.z);
				glVertex3f(-bound.half_size.x, -bound.half_size.y, bound.half_size.z);
				glVertex3f( bound.half_size.x, -bound.half_size.y, bound.half_size.z);
				glVertex3f( bound.half_size.x,  bound.half_size.y, bound.half_size.z);
				
				//left side
				glVertex3f(-bound.half_size.x, bound.half_size.y,-bound.half_size.z);
				glVertex3f(-bound.half_size.x,-bound.half_size.y,-bound.half_size.z);
				glVertex3f(-bound.half_size.x,-bound.half_size.y, bound.half_size.z);
				glVertex3f(-bound.half_size.x, bound.half_size.y, bound.half_size.z);

				//right side
				glVertex3f( bound.half_size.x, bound.half_size.y,-bound.half_size.z);
				glVertex3f( bound.half_size.x,-bound.half_size.y,-bound.half_size.z);
				glVertex3f( bound.half_size.x,-bound.half_size.y, bound.half_size.z);
				glVertex3f( bound.half_size.x, bound.half_size.y, bound.half_size.z);

				glEnd();
				glPopMatrix();

			}	
		}
	}

	render_group.clear();
}

// @notworking
inline glm::mat4 to_billboard(glm::vec3 position,glm::vec3 cameraPos,glm::vec3 cameraUp) {
    glm::vec3 look = glm::normalize(cameraPos - position);
    glm::vec3 right = glm::cross(cameraUp, look);
    glm::vec3 up2 = glm::cross(look, right);
    glm::mat4 transform;
    transform[0] = glm::vec4(right, 0);
    transform[1] = glm::vec4(up2, 0);
    transform[2] = glm::vec4(look, 0);
    transform[3] = glm::vec4(position, 0);

	transform = glm::rotate(transform, 90.f, glm::vec3(0, 1, 0));

    return transform;
}

void Renderer::End_3D(Camera & camera) {
	// end the 3d rendering
	// unbind the shader
	static_shader.Unset();

	// adds a pyrimid over each light
	// @refactor
	if (!debug) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(glm::value_ptr(projection_matrix));

	static float rot = 0;
	rot += 1.f;

	float size = 1;

	std::vector<float> vert {
		0, 0, -1,
		0, 2, 0,
		0.866025, 0, 0.5,
		-0.866025, 0, 0.5
	};
	std::vector<short> in{
		2, 1, 3,
		0, 1, 2,
		3, 1, 0,
		0, 2, 3
	};

	// @fix, this is done using the fixed function pipeline, i need to make this modern sometime soon 
	for (auto light : lights){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushMatrix();
		glLoadMatrixf(glm::value_ptr(camera.Get_View_Matrix()));

		glTranslatef(light.Position.x, light.Position.y, light.Position.z);
		glRotatef(rot,0.f, 1.f, 0.f);

		glDisable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLES);
		glColor3f(1, 1, 1);

		// pyrimid
		for (unsigned int i = 0; i < in.size(); i++){
			float x = vert[3 * in[i] + 0];
			float y = vert[3 * in[i] + 1];
			float z = vert[3 * in[i] + 2];

			glColor3f( x, y, z);
			glVertex3f(x * size, y * size, z * size);
		}

		glEnd();

		glEnable(GL_TEXTURE_2D);
		glPopMatrix();
	}
}

// @unused
void Renderer::set_2d()
{

}

void Renderer::set_3d()
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
}
