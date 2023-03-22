#include<Texture.h>
#include<stb_image/stb_image.h>
#include<Debug.h>


Texture::Texture(unsigned int slots)
{
	if (slots <= 0)	//cannot really be smaller, just in case I mess things up
	{
		slots = 1;
	}
	slots > GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ? this->idCount = slots : this->idCount = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;
	//Do not use "slots" variable beyond this point, replace with this->idCount
	this->id = new GLuint[this->idCount];
	glGenTextures(this->idCount, this->id);
	for (int i = 0; i < this->idCount; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);	//yes, this is the correct way of calling this function, according to Nicol Bolas: https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
		glBindTexture(GL_TEXTURE_2D, this->id[i]);

		//setting, figure out some way of customizing these

		//OK so, I'm starting to think that maybe these settings are tied to the texture UNITS ("slots") and NOT the texture object itself
		//if this is the case then these settings need to be saved with each texture object and re-applied every... single... draw-call...
		//unless we use some clever abstraction IDK
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

}

Texture::~Texture()
{
	delete this->id;
}