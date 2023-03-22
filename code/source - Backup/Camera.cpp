//#include <Camera.h>	//included in Cameraman.h
#include <Cameraman.h>
#include <iostream>
#include <glm/gtc/matrix_inverse.hpp>
#include <Debug.h>

//using only default values (both arguments defaulted to nullptr)
//TODO: extend to modify transform according to arguments (if any)
Camera::Camera(std::string name, glm::vec3* location, glm::vec3* target) : name(name)
{
	//this->viewMatrix = glm::perspective(45.0f, (GLfloat)winWid / (GLfloat)winHei, 0.1f, 1000.0f);
	switch (Cameraman::defaultCameraType)
	{
	case Cameraman::cameraType::UNSET:
		//error message
		LOGERROR("unset cameraType used in cameraConstructor");
		break;

	case Cameraman::cameraType::PERSPECTIVE:
		this->projectionMatrix = glm::perspective(Cameraman::defaultPerspectiveYangle, Cameraman::defaultAspectRatio, 0.1f, 1000.0f);
		this->projReady = true;
		break;

	case Cameraman::cameraType::ISOMETRIC:
		//isometric camera here
		LOGERROR("isometirc cameraType used in cameraConstructor(not implemented)");
		break;
	}
	registerCamera();
}

Camera::~Camera()
{
	unregisterCamera();
}

//projection * view
glm::mat4* Camera::getVPMatrix()
{
	if (this->matrixReady)	//if unmoved since last calculation
		return &this->cameraMatrix;

	//calculate view matrix based on transform
	this->viewMatrix = glm::affineInverse(*(this->getMatrix()));

	//combine with projection matrix
	this->cameraMatrix = this->projectionMatrix * this->viewMatrix;

	return &this->cameraMatrix;
}

void Camera::registerCamera()
{
	//if ringlist is empty, initialize with newCam
	if (Cameraman::firstCamera == nullptr)
	{
		this->nextCam = this;
		this->prevCam = this;
		Cameraman::firstCamera = this;
	}

	else
	{
		//otherwise, check that newCam is not already registered
		if (this->nextCam != nullptr || this->prevCam != nullptr)
		{
			LOGWARNING("Camera \"" + this->name + "\" has already been registered!");
			//std::cout << "WARNING: Camera \"" << this->name << "\" has already been registered!" << std::endl;			//(DEBUG) REPLACE WITH PROPER DEBUG SYSTEM
			return;
		}
		//if not add last in the list (firstCamera.prev) TAKE EXTREME CARE
		//link in new camera
		this->nextCam = Cameraman::firstCamera;
		this->prevCam = Cameraman::firstCamera->prevCam;
		//re-link last camera
		this->prevCam->nextCam = this;
		//re-link first camera
		this->nextCam->prevCam = this;
	}
}


//there is probably a smarter way to do this...
void Camera::unregisterCamera()
{
	//if camera isn't part of ringlist, flag warning and return
	if (this->nextCam == nullptr || this->prevCam == nullptr)
	{
		std::cout << "WARNING: Camera \"" << this->name << "\" has yet to be registered, but was marked for unregistration!" << std::endl;			//(DEBUG) REPLACE WITH PROPER DEBUG SYSTEM
		return;
	}

	//if this camera is the ONLY registered camera
	if (this->nextCam == this && Cameraman::firstCamera == this)
	{
		//reset ringlist state and return
		Cameraman::firstCamera = nullptr;
		this->nextCam = nullptr;
		this->prevCam = nullptr;
		return;
	}

	//if this camera is the first camera
	if (Cameraman::firstCamera == this)
	{	
		//set new first camera
		Cameraman::firstCamera = this->nextCam;
	}
	//unlink camera
	//re-link next and previous cameras
	this->nextCam->prevCam = this->prevCam;
	this->prevCam->nextCam = this->nextCam;
	//remove self from ringlink
	this->nextCam = nullptr;
	this->prevCam = nullptr;
}