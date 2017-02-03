#include "shaders.h"

inline void Shader::check_compile_errors(GLuint shader, std::string type) {
	// check each shader to see if opengl returns a syntax error, might want to spice this up
	// with curses syling, but besure to make sure its only for linux
	GLint success;
	GLchar info_log[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			printf("ERROR::SHADER_COMPILATION:: %s: %s\n", type.c_str(), info_log);
		}
	}
}

Shader::Shader(){}

void Shader::Load_Shaders(const char * v_file,const char * f_file) {
	// Load the shader and create a program, then bind each shader to the program
	vertex_id	= Load_Shader(v_file, GL_VERTEX_SHADER);
	fragment_id = Load_Shader(f_file, GL_FRAGMENT_SHADER);

	program_id = glCreateProgram();
	glAttachShader(program_id, vertex_id);
	glAttachShader(program_id, fragment_id);

	// make sure that the program created correctly, might want to throw an error if the program_id
	// is equal to zero or something.
	glLinkProgram(program_id);
	glValidateProgram(program_id);

	bind_attributes();
}

Shader::~Shader() {
	// destroy the shader 
	Unset();

	// detach, not really needed but done for safety
	glDetachShader(program_id, vertex_id);
	glDetachShader(program_id, fragment_id);

	// delete all of the ids
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
	glDeleteProgram(program_id);
}

GLuint Shader::Load_Shader(const char * file, GLenum type) {
	// create a handle for the file, @todo check that the file is in the directory!
	std::ifstream shader_file;

	// throw exception if file contains wrong data
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	shader_file.open(file); // open the file handle

	// stream the file into a stringstream, it contains all of the text in the file
	std::stringstream stream;
	stream << shader_file.rdbuf();
	shader_file.close();

	// convert the code into a c string, @refactor
	auto code = stream.str();
	const GLchar * ccode = code.c_str();

	// create the shader and compile
	GLuint shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &ccode, NULL);
	glCompileShader(shader);

	// check for errors
	check_compile_errors(shader, ((type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT"));

	// return the shader id
	return shader;
}

// tell opengl to use the shader
void Shader::Set()   { glUseProgram(program_id); }

// resets the shader used by opengl
void Shader::Unset() { glUseProgram(0); }

void Shader::bind_attribute(int attrib,const char * name) {
	// binds a atribute to an id in the shader code, for example: 
	// vertex.glsl:
	// in vec3 vertices;
	glBindAttribLocation(program_id, attrib, name);
}

void Shader::Set_Uniform(const char * name, float v[], int num) {
	// get the uniform location id
	GLuint loc = glGetUniformLocation(program_id, name);

	// check which dimension the vector uses and passes that as a uniform into 
	// the currently bound shader
	switch (num) {
	case 2: glUniform2f(loc, v[0], v[1]);				return;
	case 3: glUniform3f(loc, v[0], v[1], v[2]);			return;
	case 4: glUniform4f(loc, v[0], v[1], v[2], v[3]);	return;
	}
}

void Shader::Set_Uniform(const char * name, glm::mat4 m) {
	// get the uniform matrix location in the shader
	GLuint loc = glGetUniformLocation(program_id, name);

	// glm::value_ptr converts the matrix into a float [16], it also transposes the matrix
	// into something that opengl can read
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::Set_Uniform(const char * name, float n){
	// used for float uniforms
	GLuint loc = glGetUniformLocation(program_id, name);
	glUniform1f(loc, n);
}