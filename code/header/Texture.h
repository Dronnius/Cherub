#pragma once

#include <glew/glew.h>
#include <glfw/glfw3.h>
//#include <functional>		//I'm obstinate and want to try without

class Texture
{
private:

	struct image
	{
		GLuint texId = 0;		//openGL id of texture object
		unsigned char* data = nullptr;
		int width = 0;		//width of image, 0 indicates invalid
		int height = 0;	//height of image, 0 indicates invalid
		int channels = 0;	//bits per pixel???, 0 indicates invalid

		image() {};	//standard constructor
		/*image(GLuint id, unsigned int width, unsigned int height)	//unused
		{
			this->id = id;
			this->width = width;
			this->height = height;
		}*/
	};
	//GLuint* id = nullptr;
	image* images = nullptr;
	unsigned int idCount;



public:
	unsigned int uses = 0;	//keeps track of number of visual objects referencing this texture, incremented and decremented by Visual
	/*struct configUnit		//save it for later
	{

	};*/

	//set up (slots) textures
	//arg1, "slots", determines the number of textures to be used. TODO: make sure this works even if "slots" exceeds maximum openGL texture units
	//arg2, is an array of char strings, the number of char string MUST equal "slots"
	Texture(unsigned int slots, char** filenames);

	~Texture();

	GLuint getTextureID(unsigned int index)
	{
		if (images == nullptr || index >= idCount)
			return 0;
		return images[index].texId;
	}

	void bind();
	void unbind();
};