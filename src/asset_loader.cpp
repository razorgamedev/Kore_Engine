#include "asset_loader.h"

Texture asset_loader::Load_Texture(const char * path, GLenum filter, bool repeat, float _shine, float _reflect) {
	
	double start_time = glfwGetTime();

	GLuint id = SOIL_load_OGL_texture(
		path,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS			|
		SOIL_FLAG_INVERT_Y			| 
		SOIL_FLAG_NTSC_SAFE_RGB		| 
		SOIL_FLAG_COMPRESS_TO_DXT   
	);

	if(id == 0) {
		printf("ERROR::ASSET_LOADER:: %s\n", SOIL_last_result());
	}
	
	glBindTexture(GL_TEXTURE_2D,id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	int w, h;
	int miplevel = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);

	auto t = Texture();
	t.ID              = id;
    t.Width           = w;
    t.Height          = h;
    t.Shine           = _shine;
    t.Reflectivity    = _reflect;
	t.Path			  = std::string{path};

	std::cout << "Loaded texture: " << path << " elapsed time: " << glfwGetTime()-start_time << std::endl;

	return t;
}


GLuint asset_loader::Load_Texture_ID_Async(GLFWwindow * window,const char * path){
	glfwMakeContextCurrent(window);
	GLuint id = SOIL_load_OGL_texture(
		path,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS			|
		SOIL_FLAG_INVERT_Y			| 
		SOIL_FLAG_NTSC_SAFE_RGB		| 
		SOIL_FLAG_COMPRESS_TO_DXT   
	);
	
	if(id == 0) {
		printf("ERROR::ASSET_LOADER:: %s\n", SOIL_last_result());
	}
	return id;
}

Texture asset_loader::Create_Texture_From_ID(GLuint id, const char * path, GLenum filter, bool repeat, float _shine, float _reflect){
	
	glBindTexture(GL_TEXTURE_2D,id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	if (repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	int w, h;
	int miplevel = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);

	auto t = Texture();
	t.ID              = id;
    t.Width           = w;
    t.Height          = h;
    t.Shine           = _shine;
    t.Reflectivity    = _reflect;
	t.Path			  = std::string{path};

	return t;
}