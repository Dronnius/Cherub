#pragma once
#include <Camera.h>
#include <glew/glew.h>
#include <glfw/glfw3.h>
/*#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"*/

//<Camera.h> <Transform.h> <string> "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp" <glew/glew.h> <glfw/glfw3.h>

//forward declaration
//class Camera;

//static
class Cameraman
{
private:
	static Camera* activeCamera;
	static glm::vec3 defaultUpVector;

public:
	static Camera* firstCamera;
	//represents the default cameratype, perspective or isometric, along with relevant variables
	static Camera::cameraType defaultCameraType;
	//float defaultPerspectiveXangle = 45.0;
	static float defaultPerspectiveYangle;
	static float defaultAspectRatio;

	static void setActiveCamera(Camera* newActiveCamera);

	//returns true if argument 1 points to the currently active camera, otherwise false
	static bool isActiveCamera(Camera* camera);

	//initializes default values, call after setting up glfw and windows size
	static void initialize(GLFWwindow* window);

	//call this from shaders, will get the current projection*view matrix of the active camera
	static glm::mat4* film();

	//returns nullptr if camera wasn't found
	static Camera* getCameraByName(std::string target);

	//this member and variable, and related function, represent the default euclidean "up", as represented on the screen

	static void setUpX() { defaultUpVector = glm::vec3(1.0f, 0.0f, 0.0f); }
	static void setUpY() { defaultUpVector = glm::vec3(0.0f, 1.0f, 0.0f); }
	static void setUpZ() { defaultUpVector = glm::vec3(0.0f, 0.0f, 1.0f); }
	static void setUpV(glm::vec3* newUp) { defaultUpVector = *newUp; }

};