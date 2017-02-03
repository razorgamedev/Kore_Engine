#ifndef __FPS_CAMERA_H__
#define __FPS_CAMERA_H__

// Std. Includes
#include <vector>
#include <GLFW/glfw3.h>

// GL Includes
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "renderer.h"
#include "scene_3d.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 4.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	double m_last_x;
	double m_last_y;

	double m_current_x;
	double m_current_y;

	int r_last_state, r_current_state;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		r_last_state = r_current_state = GLFW_RELEASE;
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();

		m_current_x = m_current_y = m_last_x = m_last_y = 0;
	}
	// Constructor with scalar values
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		r_last_state = r_current_state = GLFW_RELEASE;
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();

		m_current_x = m_current_y = m_last_x = m_last_y = 0;
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 Get_View_Matrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}

	glm::vec3 Calculate_Mouse_Ray(GLFWwindow * window, glm::mat4 projection){
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int w_width, w_height;
		glfwGetWindowSize(window, &w_width, &w_height);

		float mx = (float) xpos / (w_width * 0.5f) - 1.0f;
		float my = (float) ypos / (w_height * 0.5f) - 1.0f;	
		
		auto view = glm::lookAt(glm::vec3(0.0f), Front, Up);

		glm::mat4 inv_vp = glm::inverse(projection * view);
		glm::vec4 screen_pos = glm::vec4(mx, -my, 1.0f, 1.0f);
		glm::vec4 world_pos = inv_vp * screen_pos;

		return glm::normalize(glm::vec3(world_pos));
	}

	std::vector<glm::vec3> Get_View_Frustum(float near, float far, glm::vec2 near_size, glm::vec2 far_size){
		auto near_center 	= Position + (Front * near);
		auto far_center 	= Position + (Front * far);

		auto relative_right = glm::normalize(glm::cross(Front, Up));
		auto local_up		= glm::normalize(glm::cross(Front, relative_right));

		auto near_left_top  	= near_center - relative_right * near_size.x + local_up * near_size.y;
		auto near_left_bottom	= near_center - relative_right * near_size.x - local_up * near_size.y;
		auto near_right_top 	= near_center + relative_right * near_size.x + local_up * near_size.y;
		auto near_right_bottom 	= near_center + relative_right * near_size.x - local_up * near_size.y;

		auto far_left_top  		= far_center - relative_right * far_size.x + local_up * far_size.y;
		auto far_left_bottom	= far_center - relative_right * far_size.x - local_up * far_size.y;
		auto far_right_top 		= far_center + relative_right * far_size.x + local_up * far_size.y;
		auto far_right_bottom 	= far_center + relative_right * far_size.x - local_up * far_size.y;

		std::vector<glm::vec3> planes;
		planes.push_back(near_left_top);
		planes.push_back(near_left_bottom);

		planes.push_back(near_right_top);
		planes.push_back(near_right_bottom);
		
		planes.push_back(far_left_top);
		planes.push_back(far_left_bottom);
		
		planes.push_back(far_right_top);
		planes.push_back(far_right_bottom);
		return planes;
	}

	void Update(float dt, GLFWwindow * window) {
		MovementSpeed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) ? SPEED * 4 : SPEED;
		
		if (glfwGetKey(window, GLFW_KEY_W))
			ProcessKeyboard(Camera_Movement::FORWARD, dt);
		else if (glfwGetKey(window, GLFW_KEY_S))
			ProcessKeyboard(Camera_Movement::BACKWARD, dt);
		else if (glfwGetKey(window, GLFW_KEY_A))
			ProcessKeyboard(Camera_Movement::LEFT, dt);
		else if (glfwGetKey(window, GLFW_KEY_D))
			ProcessKeyboard(Camera_Movement::RIGHT, dt);
		
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
			this->Position.y -= MovementSpeed * 0.05f;
		else if (glfwGetKey(window, GLFW_KEY_SPACE))
			this->Position.y += MovementSpeed * 0.05f;

		r_current_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		if (r_current_state == GLFW_PRESS){

			glfwGetCursorPos(window, &m_current_x, &m_current_y);
			
			if (r_last_state == GLFW_RELEASE){
				m_last_x = m_current_x;
				m_last_y = m_current_y;
			}
			
			auto m_delta_x = m_current_x - m_last_x;
			auto m_delta_y = m_current_y - m_last_y;
			m_last_x = m_current_x;
			m_last_y = m_current_y;

			// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			ProcessMouseMovement((float)m_delta_x, (float)-m_delta_y);
		}
		r_last_state = r_current_state;
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;

		auto front = glm::vec3(Front.x, 0, Front.z);
		if (direction == FORWARD)
			this->Position += front * velocity;
		if (direction == BACKWARD)
			this->Position -= front * velocity;
		if (direction == LEFT)
			this->Position -= this->Right * velocity;
		if (direction == RIGHT)
			this->Position += this->Right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= this->MouseSensitivity;
		yoffset *= this->MouseSensitivity;

		this->Yaw 	+= xoffset;
		this->Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;
	}

private:
	double speed;

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
	}

	glm::vec3 current_ray;
};

#endif // __FPS_CAMERA_H__