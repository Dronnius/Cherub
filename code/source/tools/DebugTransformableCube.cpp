#include "tools/DebugTransformableCube.h"
#include "Debug.h"
#include <iterator>
#include <functional>


std::vector<DebugTransformableCube> cubes;

void DebugTransformableCube::createCube(std::string name, float timestamp,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
	Transform* parentTransform, Texture* texture)
{
	cubes.push_back(DebugTransformableCube(name, timestamp, position, rotation, scale, parentTransform, texture));
}

DebugTransformableCube::DebugTransformableCube(std::string name, float timestamp,
	glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
	Transform* parentTransform, Texture* texture)
	:
	Logical(name, timestamp),
	Visual()
{
	//nothing
}

DebugTransformableCube::~DebugTransformableCube()
{
	//I don't need to do this in destructor because I'm allocating the objects in the vector to begin with
	//std::vector should handle removing items

	//bool foundMyself = false;
	////REMEMBER TO REMOVE FROM VECTOR
	//for (std::vector<DebugTransformableCube>::iterator iter = cubes.begin(); iter != cubes.end(); iter++)
	//{
	//	if (iter->getName().compare(this->name) == 0)
	//	{
	//		foundMyself = true;
	//		iter = cubes.erase(iter);
	//	}
	//}
	//if (!foundMyself)
	//	LOGWARNING("Could not find and delete myself (debugTransformableCube: " + this->name +"from the vector of cubes!\n\t(potential memory leak)");

}

bool DebugTransformableCube::commandTransformCube(std::vector<std::string> args)
{
	const std::string helpMsg = "Please provide a name, a transformation identifier & relevant arguments, provide vector arguments as comma-separated lists:" \
		"\n\ttfcb [name] [transformation] ([argumentA] [argumentB] [vector_x],[vector_y]...)";
	if (args.size() < 3)
	{
		std::cout << helpMsg << std::endl;
		return false;
	}

	bool argumentsOK = true;

	DebugTransformableCube* target = nullptr;

	for (std::vector<DebugTransformableCube>::iterator iter = cubes.begin(); iter != cubes.end(); iter++)
	{
		if (iter->getName().compare(args[1]) == 0)
		{
			target = &(*iter);		//obtain pointer
			break;
		}
	}
	if (!target)
	{
		std::cout << "Name did not match any existing cube" << std::endl
			<< helpMsg << std::endl;
		argumentsOK = false;
	}

	struct commandNameFunctionPair {
		std::string functionName = ""; 
		void (Transform::*vecFunc)(glm::vec3) = nullptr; 
		void (Transform::*vefFunc)(glm::vec3, float) = nullptr; 
		void (Transform::*quatFunc)(glm::quat) = nullptr;
		
		//sloppy but simple solution, add further function types as constructors here
		commandNameFunctionPair(std::string functionName, void (Transform::*vecFunc)(glm::vec3)) { this->functionName = functionName; this->vecFunc = vecFunc; }
		commandNameFunctionPair(std::string functionName, void (Transform::*vefFunc)(glm::vec3, float)) { this->functionName = functionName; this->vefFunc = vefFunc; }
		commandNameFunctionPair(std::string functionName, void (Transform::*quatFunc)(glm::quat)) { this->functionName = functionName; this->quatFunc = quatFunc; }
		//std::function<void(glm::vec3)> vecFunc = nullptr;
		//std::function<void(glm::vec3, float)> vefFunc = nullptr;
		//std::function<void(glm::quat)> quatFunc = nullptr;
		//
		//commandNameFunctionPair(std::string functionName, void (Transform::* vecFunc)(glm::vec3), Transform* t) 
		//	{ this->functionName = functionName; this->vecFunc = std::bind(vecFunc, t, std::placeholders::_1); }
		//commandNameFunctionPair(std::string functionName, std::function<void(glm::vec3, float)> vefFunc) { this->functionName = functionName; this->vefFunc = vefFunc; }
		//commandNameFunctionPair(std::string functionName, std::function<void(glm::quat)> quatFunc) { this->functionName = functionName; this->quatFunc = quatFunc; }

	};

	//static commandNameFunctionPair functionList[] = {
	//	commandNameFunctionPair("setPos", &(target->*setPosition))
	//};
	//std::function<void(glm::vec3)> a = std::bind(&Transform::setPosition, std::placeholders::_1);		//uncommenting this line breaks intellisense (???)
	static commandNameFunctionPair functionList[] = {
		commandNameFunctionPair("setPos", &Transform::setPosition),
		commandNameFunctionPair("setRot", static_cast<void (Transform::*)(glm::quat)>(&Transform::setRotation)),	//static cast due to ambiguous parameters
		commandNameFunctionPair("setScl", &Transform::setScale)
	};

	//find target function
	commandNameFunctionPair* functionPair = nullptr;
	unsigned int parameterType = -1;
	for (int i = 0; i < sizeof(functionList) / sizeof(commandNameFunctionPair); i++)
	{
		if (functionList[i].functionName.compare(args[2]) == 0)
		{
			functionPair = &functionList[i];
			if(functionPair->vecFunc != nullptr)
				parameterType = 0;
			else if (functionPair->vefFunc != nullptr)
				parameterType = 1;
			else if (functionPair->quatFunc != nullptr)
				parameterType = 2;
			else
				parameterType = -1;
		}
	}
	if (functionPair == nullptr)
	{
		std::cout << "Transformation did not match any registered function pair" << std::endl
			<< helpMsg << std::endl;
		argumentsOK = false;
	}
	else if (parameterType == -1)
		LOGWARNING("Failed to register a valid function");


	//parse arguments and call function
	//float floatArgument;
	//glm::quat quaternionArgument;
	
	if (!argumentsOK)
		return false;

	//just hard-code this
	switch (parameterType)
	{
	case 0:
		if (args.size() < 4) {
			std::cout << "This function requires a vector argument: tfcb [name] [transformation] [vector_x],[vector_y],[vector_z]" << std::endl;
			break;
		}
		try {
			glm::vec3 vectorArgument = sParseVec3(args[3]);
			(target->*(functionPair->vecFunc))(vectorArgument);
		}
		catch (std::invalid_argument& e) { 
			std::cout << "Incorrectly formatted arguments. Write vectors as [vector_x],[vector_y],[vector_z], example: \"1,1,-1\"" << std::endl;
		}
		break;
	case 1:
		//vector and float call
		break;
	case 2:
		//quaternion call (good luck)
		break;
	default:
		break;
	}
	
	return true;
}
bool DebugTransformableCube::commandAddRemoveCube(std::vector<std::string> args)
{
	if (args.size() < 2)
	{
		std::cout << "Please provide a name: arcb [name]" << std::endl;
		return false;
	}
	LOGNOTE("Missing timer implementation");
	bool nameInUse = false;
	for (std::vector<DebugTransformableCube>::iterator iter = cubes.begin(); iter != cubes.end(); iter++)
	{
		if (iter->getName().compare(args[1]) == 0)
		{
			nameInUse = true;
			iter = cubes.erase(iter);
			std::cout << "deleted cube: " << args[1] << std::endl;
		}
	}
	if (!nameInUse)
	{
		createCube(args[1], 0.0f);
		std::cout << "created cube: " << args[1] << std::endl;
	}
	return true;
}

bool DebugTransformableCube::commandListCubes(std::vector<std::string> args)
{
	std::cout << "active debug cubes (name, position):" << std::endl;
	for (std::vector<DebugTransformableCube>::iterator iter = cubes.begin(); iter != cubes.end(); iter++)
	{
		std::cout << "\t" << iter->name << "\t\t" << Visual::toString(iter->getPosition()) << std::endl;
	}
	return true;
}

void DebugTransformableCube::addConsoleCommands()
{
	addToConsoleCommandList(Command("tfcb", commandTransformCube, "transform a chosen debug cube by name, or all at once"));
	addToConsoleCommandList(Command("arcb", commandAddRemoveCube, "add or remove a debug cube by name"));
	addToConsoleCommandList(Command("lscb", commandListCubes, "list debug cubes by name and position"));
}

int DebugTransformableCube::update(float dTime) { return 0; }	//does nothing