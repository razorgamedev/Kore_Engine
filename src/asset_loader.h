#ifndef __ASSET_LOADER_H__
#define __ASSET_LOADER_H__

#include <GL/glew.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <vector>

#include "texture.h"

namespace asset_loader {
	Texture Load_Texture(const char * path, GLenum filter = GL_LINEAR, bool repeat = false, float _shine = 10.f, float _reflect = 1);

	GLuint Load_Texture_ID_Async(GLFWwindow * window,const char * path);
	Texture Create_Texture_From_ID(GLuint id, const char * path, GLenum filter = GL_LINEAR, bool repeat = false, float _shine = 10.f, float _reflect = 1);
}

#endif // __ASSET_LOADER_H__
