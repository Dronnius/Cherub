#include <Cameraman.h>
//#include <glew/glew.h>
//#include <glfw/glfw3.h>
#include <iostream>
#include <Debug.h>


//static member variables, initialized here
Camera* Cameraman::activeCamera = nullptr;
glm::vec3 Cameraman::defaultUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

Camera::cameraType Cameraman::defaultCameraType = Camera::cameraType::UNSET;
float Cameraman::defaultPerspectiveYangle = 0.0;
float Cameraman::defaultAspectRatio = 0.0;
Camera* Cameraman::firstCamera = nullptr;

void Cameraman::initialize(GLFWwindow* window)
{
	defaultCameraType = Camera::cameraType::PERSPECTIVE;
	int winWid, winHei;
	glfwGetWindowSize(window, &winWid, &winHei);
	defaultPerspectiveYangle = 45.0;
	defaultAspectRatio = (GLfloat)winWid / (GLfloat)winHei;
}


void Cameraman::setActiveCamera(Camera* newActiveCamera)
{
	activeCamera = newActiveCamera;
	LOGNOTE("Set camera \"" + newActiveCamera->name + "\" as the new active camera.");
	//std::cout << "NOTE: Set camera \"" << newActiveCamera->name << "\" as the new active camera." << std::endl;		//DEBUG
}


glm::mat4* Cameraman::film()
{
	if (Cameraman::activeCamera == nullptr)
		if(Cameraman::firstCamera == nullptr)
			return nullptr;
		else
		{
			setActiveCamera(Cameraman::firstCamera);
			LOGWARNING("No active camera! emergency fix -> found first registered camera, using firstCamera");
			//std::cout << "WARNING: No active camera! emergency fix -> found first registered camera, using firstCamera" << std::endl;			//(DEBUG) REPLACE WITH PROPER DEBUG SYSTEM
		}
	return activeCamera->getVPMatrix();
}

Camera* Cameraman::getCameraByName(std::string target)
{
	Camera* examiner = Cameraman::firstCamera;
	do
	{
		if (examiner->name.compare(target) == 0) //if found
		{
			return examiner;
		}
		if (examiner->nextCam == nullptr)	//if ringlink is broken
		{
			LOGALERT("Camera \"" + examiner->name + "\" has a broken (next) link!");
			//std::cout << "ERROR: Camera \"" << examiner->name << "\" has a broken (next) link!" << std::endl;			//(DEBUG) REPLACE WITH PROPER DEBUG SYSTEM
			break;
		}
	} while ((examiner = examiner->nextCam) != Cameraman::firstCamera);
	return nullptr;
}


//returns true if argument 1 points to the currently active camera, otherwise false
bool Cameraman::isActiveCamera(Camera* camera)
{
	return Cameraman::activeCamera == camera;
}