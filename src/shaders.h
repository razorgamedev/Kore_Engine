#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader {
public:
	Shader();
	~Shader();

	void Load_Shaders(const char * v_file,const char * f_file);
	GLuint Load_Shader(const char * file, GLenum type);

	void Set();
	void Unset();

	void Set_Uniform(const char * name, float v[], int num = 4);
	void Set_Uniform(const char * name, glm::mat4 m);
	void Set_Uniform(const char * name, float n);

	virtual void Load() {};
	virtual void bind_attributes() {};

protected:
	void bind_attribute(int attrib,const char * name);

private:
	GLuint program_id, vertex_id, fragment_id;

	inline void check_compile_errors(GLuint shader, std::string type);
};

#endif // __SHADER_H__
