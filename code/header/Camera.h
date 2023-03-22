#pragma once
#include <Transform.h>
#include <string>

//<Transform.h> <string> "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp"

//#include <Cameraman.h>	//look out for circular dependency

//if you want a camera with an update function, for instance one that follows a player character,
// make a subclass that inherits from Camera and Logical.
// or, make modify a regular camera object from the character's update function

//camera looks in positive z axis.
// by default, it will try to maintain positive y axis upward (this will ve difficult)


//regarding transform, we want to do the opposite operations (move camera left -> move objects right)
//TODO: implement the "view" part of projection*view

//this is entirely handled by the Cameraman static class. Only here for reference purposes... and to avoid circular dependencies. TRUST ME THIS IS EASIER
//ring-list structure of registered cameras
/*struct Cam
{
	Camera* camera = nullptr;
	std::string name;	//in case a camera needs to be retrieved by name
	Cam* next = nullptr;
	Cam* prev = nullptr;
};*/

class Camera : public Transform
{
private:
	//make sure camera maintainst this vector as "upward" on the screen, when projected
	// nullptr implies that camera has default up-direction, refer to camera manager instead. (typical state)
	glm::vec3* up = nullptr;
	int xValue = 0;	//0 means refer to default
	int yValue = 0;	//0 means refer to default

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	bool projReady = false;	//probably not needed
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	bool viewReady = false;	
	glm::mat4 cameraMatrix = glm::mat4(1.0f);	//(correct) combination of projection and view matrix
	bool matrixReady = false;

	//register the camera in camera constructor
	void registerCamera();
	//unregister the camera in camera destructor
	void unregisterCamera();

public:
	//This used to be part of a stupid "Cam" struct, but this makes more sense
	//These variables are managed HERE, in the CAMERA class, to manage a ringlist of cameras
	//The static class cameraman class has a reference to the first camera, and functions to get references to cameras and film and such
	//if either nextCam or prevCam are "nullptr", the camera is unregistered (which means register it, ASAP)
	std::string name;
	Camera* nextCam = nullptr;
	Camera* prevCam = nullptr;

	//enumerator for different kinds of cameras, you'll need to add functionality if you want to edit the FoV and such
	enum class cameraType { UNSET = -1, DEFAULT = 0, PERSPECTIVE = 1, ISOMETRIC = 2 };

	//create a default camera (defaults can be edited in camera manager)
	//first argument optional, but highly recommended
	Camera(std::string name = "Unnamed Camera", cameraType cameraType = cameraType::DEFAULT, glm::vec3* location = nullptr, glm::vec3* target = nullptr);
	//Camera(std::string name = "Unnamed Camera", glm::vec3* location = nullptr, glm::vec3* target = nullptr);
	~Camera();

	//Don't call this from shaders, refer to Cameraman::film()
	glm::mat4* getVPMatrix();

	//create a camera with a non-default stats	(TODO)
	//Camera(type, xValue, yValue);

	//rotate camera to look at target,	(TODO)
	//make use of transform pointAt		(ALSO TODO)
	//except maybe not, rotating the camera means rotating the world in the opposite direction, the camera remains static.
	//pointAt(Transform* target);

};