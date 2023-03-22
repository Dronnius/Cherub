#include<Texture.h>
#include<stb_image/stb_image.h>
#include<Debug.h>


//TODO:
//OK so I'm simply going to add doubles of certain vertices, I'm thinking 4 vertices per cube face
//Look into Sampler objects, we might need those too: http://docs.gl/gl4/glBindSampler https://www.reddit.com/r/opengl/comments/4pn5wo/please_clarify_terms_texture_sampler_vs_texture/

Texture::Texture(unsigned int slots, char** filenames)
{
	stbi_set_flip_vertically_on_load(true);		//because png's and openGL just can't agree
	if (slots <= 0)	//cannot really be smaller, just in case I mess things up
	{
		slots = 1;
	}
	slots <= GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ? this->idCount = slots : this->idCount = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS;	//TODO: make sure this works even if "slots" exceeds maximum openGL texture units
	//Do not use "slots" variable beyond this point, replace with this->idCount
	this->images = new image[this->idCount];
	GLuint* texIdArray;	//temporary variable
	texIdArray = new GLuint[this->idCount];
	glGenTextures(this->idCount, texIdArray);
	for (int i = 0; i < this->idCount; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);	//yes, this is the correct way of calling this function, according to Nicol Bolas: https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
		glBindTexture(GL_TEXTURE_2D, texIdArray[i]);
		CHECK("Constructor");

		//setting, figure out some way of customizing these

		//OK so, I'm starting to think that maybe these settings are tied to the texture UNITS ("slots") and NOT the texture object itself
		//if this is the case then these settings need to be saved with each texture object and re-applied every... single... draw-call...
		//unless we use some clever abstraction IDK
		/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/

		CHECK("Constructor");

		//load texture
		//int width, height, channels;
		this->images[i].data = stbi_load(filenames[i], &(this->images[i]).width, &(this->images[i]).height, &(this->images[i]).channels, 0);	//last argument is desired channels, not sure what channels even means...
		if (this->images[i].data)	//load successful
		{
			//this->images[i].width = width;
			//this->images[i].height = height;
			//this->images[i].channels = channels;
			this->images[i].texId = texIdArray[i];

			//http://docs.gl/gl4/glTexImage2D
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->images[i].width, this->images[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->images[i].data);	//generates the texture
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			LOGERROR("texture load failed");
		}

		CHECK("Constructor");
	}

	//cleanup
	delete[] texIdArray;
}

Texture::~Texture()
{
	//delete this->id;
	delete images;
}


void Texture::bind()
{
	for (int i = 0; i < this->idCount; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, this->images[i].texId);
	}
}

void Texture::unbind()
{
	for (int i = 0; i < this->idCount; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}