#pragma once
#include <Cameraman.h>	//includes Camera.h
#include <Logical.h>	//<string> <iostream>
#include <Input.h>

//<string> <iostream> <Camera.h> <Transform.h> <string> "glm/glm.hpp" "glm/gtc/matrix_transform.hpp" "glm/gtc/type_ptr.hpp" <glew/glew.h> <glfw/glfw3.h> <RBTree.h>

class DebugCamera : public Camera, public Logical
{
private:
	Input::ButtonTicket* cameraUp, * cameraLeft, * cameraDown, * cameraRight, * cameraShift, * cameraFaster, * cameraSlower;
	float speed = 0.5f, modifier = 1.5f;

public:
	//constructor, sets camera name to "Debug Camera"
	DebugCamera(float timeStamp = 0.0f);


	//Call update each frame
	// Arguments:
	//  dTime - time in milliseconds(?) since last frame
	// Returns:
	//  0 if update succeeded, nothing to report
	//  > 0 if update encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if update encountered a major issue, worth stopping program (should also log error)
	int update(float dTime);
};