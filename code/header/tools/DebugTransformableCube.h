#pragma once
#include "Visual.h"
#include "Logical.h"
#include "DebugConsoleCommand.h"
//#include "Input.h"



class DebugTransformableCube : public Visual, public Logical
{
private:
	DebugTransformableCube(std::string name, float timestamp, 
		glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), 
		Transform* parentTransform = nullptr, Texture* texture = nullptr);

public:
	static void createCube(std::string name, float timestamp,
		glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f),
		Transform* parentTransform = nullptr, Texture* texture = nullptr);
	~DebugTransformableCube();

	static bool commandTransformCube(std::vector<std::string> args);
	static bool commandAddRemoveCube(std::vector<std::string> args);
	static bool commandListCubes(std::vector<std::string> args);

	static void addConsoleCommands();


	//Call update each frame
	// Arguments:
	//  dTime - time in milliseconds(?) since last frame
	// Returns:
	//  0 if update succeeded, nothing to report
	//  > 0 if update encountered a minor issue, worth investigation (should also log warning)
	//  < 0 if update encountered a major issue, worth stopping program (should also log error)
	int update(float dTime); // = 0 means pure virtual, no definition required (makes class virtual, cannot be instantiated)
};