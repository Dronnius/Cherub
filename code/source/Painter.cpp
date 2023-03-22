#include <Painter.h>
#include <Debug.h>

#include <iostream>


int		Painter::enabledAttribsMaxValue = -1;
GLuint	Painter::vaoID = 0;
GLuint	Painter::samplerID = 0;


void Painter::initialize()
{
	//Retrieve an ID, and create it too (I think)
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	CHECK("Initialize");	//DEBUG
	glGenSamplers(1, &samplerID);
	//samplers take unit as argument, instead of currently bound... for some reason
	glBindSampler(0, samplerID);	//unit 0 is for base textures

	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//MAG filters do not use mip-maps, therefor GL_LINEAR_MIPMAP_LINEAR is invalid
}

void Painter::destroy()
{
	glDeleteVertexArrays(1, &Painter::vaoID);
	Painter::vaoID = 0;
}