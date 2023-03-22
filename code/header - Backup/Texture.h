#pragma once

#include <glew/glew.h>
#include <glfw/glfw3.h>
//#include <functional>		//I'm obstinate and want to try without

class Texture
{
private:
	GLuint* id = nullptr;
	unsigned int idCount;

public:
	/*struct configUnit		//save it for later
	{

	};*/

	//set up (slots) textures
	//arg1, "slots", determines the number of textures to be used. TODO: make sure this works even if "slots" exceeds maximum openGL texture units
	//arg2, is an array of char strings, the number of char string MUST equal "slots"
	Texture(unsigned int slots, char** );

	~Texture();

	GLuint getID(unsigned int index)
	{
		if (id == nullptr || index >= idCount)
			return 0;
		return id[index];
	}
};