#ifndef __WINDOW_UTILS_H__
#define __WINDOW_UTILS_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

inline GLFWwindow * create_window_and_init_opengl(const int width,const int height, const char * title){
    glfwInit();

	GLFWwindow * window = glfwCreateWindow(width, height, title, NULL, NULL);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	
	glfwWindowHint(GLFW_SAMPLES, 4);

	glfwSwapInterval(1);

	glfwMakeContextCurrent(window);

	glewInit();

	printf("\x1B[32mOpenGL version: %s\x1B[37m\n\n", glGetString(GL_VERSION));	

    return window;
}

#endif //__WINDOW_UTILS_H__