#include "model.h"

Model::Model() {
	// mesh->box.origin = Position;
	// mesh->box.half_size = glm::vec3(10, 10, 10);
}

Model::Model(glm::vec3 _position, glm::vec3 _rotation) {
	Position = _position;
	Rotation = _rotation;
	Scale.x = 1;
	Scale.y = 1;
	Scale.z = 1;
}

Model::~Model() {
	for (auto vbo : vbos)
		glDeleteBuffers(1, &vbo);
}

glm::vec3 Model::Get_Bounding_Box_Origin(){
	return mesh->box.origin + Position;
}

void Model::Update_Bounding_Box(){
	// mesh->box.origin = mesh->box.origin + Position;
}

void Model::Load(Mesh * _mesh, Texture _texture) {
	this->mesh = _mesh;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	bind_indices_buffer(mesh->indices);
	store_data_in_attib(0, 3, mesh->vertices);
	store_data_in_attib(1, 2, mesh->uvs);
	store_data_in_attib(2, 3, mesh->normals);

	glBindVertexArray(0);

	texture = _texture;
}

bool Model::Has_Texture() { return has_texture; }

Mesh * Model::Get_Mesh() { return mesh; }
Texture Model::Get_Texture() { return texture; }

GLuint Model::Get_Vao() { return vao; }
GLuint Model::Get_Vertex_Count() { return mesh->indices.size(); }


Bounding_Box Model::Get_Bounding_Box(){
	return mesh->box;
}

void Model::Set_Bounding_Box(Bounding_Box box){
	mesh->box = box;
}

void Model::store_data_in_attib(int num, int coord, std::vector<GLfloat> &data) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), &data[0], GL_STATIC_DRAW);
	vbos.push_back(vbo);

	glVertexAttribPointer(
		num,
		coord,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void *)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::bind_indices_buffer(std::vector<GLuint> & data) {
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLuint) * data.size(), &data[0], GL_STATIC_DRAW);
	vbos.push_back(vbo);
}

glm::mat4 Model::Get_Transform() {
	if (Scale.x == 0){
		Scale.x = 1;
		Scale.y = 1;
		Scale.z = 1;
	}

	glm::mat4 trans, rot, scale;

	trans = glm::translate(trans, glm::vec3(Position));
	trans = glm::rotate(trans, Rotation.x, glm::vec3(1, 0, 0));
	trans = glm::rotate(trans, Rotation.y, glm::vec3(0, 1, 0));
	trans = glm::rotate(trans, Rotation.z, glm::vec3(0, 0, 1));
	trans = glm::scale(trans, Scale);

	return trans;
}