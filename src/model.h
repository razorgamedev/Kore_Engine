#ifndef __MODEL_H__
#define __MODEL_H__

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"
#include "texture.h"

class Model {
public:
	Model();
	Model(glm::vec3 _position, glm::vec3 _rotation = glm::vec3());

	~Model();

	void Load(Mesh * _mesh, Texture _texture);
	//void Load_With_Collider(Mesh * )

	GLuint Get_Vao();
	GLuint Get_Vertex_Count();
	Texture Get_Texture();
	
	bool Has_Texture();

	Mesh * Get_Mesh();

	glm::mat4 Get_Transform();

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	void Update_Bounding_Box();

	glm::vec3 Get_Bounding_Box_Origin();

	Bounding_Box Get_Bounding_Box();
	void Set_Bounding_Box(Bounding_Box box);

private:
	Mesh * mesh;

	std::vector<GLuint> vbos;
	
	GLuint vao;
	Texture texture;

	bool has_texture;

	void store_data_in_attib(int num, int coord, std::vector<GLfloat> &data);
	void bind_indices_buffer(std::vector<GLuint> & data);
};

#endif // !__MODEL_H__

