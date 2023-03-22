#pragma once
#include <glew/glew.h>
#include <glfw/glfw3.h>


//this static class will manage all persistent settings related to rendering, including sampler paramaters and the vertex array object(s)
//static 
class Painter
{
private:
	//the highest index value of the currently enabled VertexAttribPointers
	//this variable is accessed and managed by Shader objects
	//At any time (except while managing shaders) this variable MUST be correct according to the description above
		//OR, be valued -1, implying that NO VAPs are enabled.
	static int enabledAttribsMaxValue;
	static GLuint vaoID;
	static GLuint samplerID;


public:
	static void initialize();

	//static const unsigned int vertexByteSize = 7 * sizeof(float);	//TEMPORARY STATIC VALUE for number of bytes per vertex, TODO: PROPER IMPLEMENTATION LATER!

	//getters & setters
	
	//the highest index value of the currently enabled VertexAttribPointers
	static int getAttribsMaxIndex()
	{
		return enabledAttribsMaxValue;
	}
	static void setAttribsMaxIndex(int value)
	{
		if (value > -2)
		{
			enabledAttribsMaxValue = value;
		}
		else enabledAttribsMaxValue = -1;
	}
	static GLuint getVaoID()
	{
		return vaoID;
	}

	static GLuint getSamplerID()
	{
		return samplerID;
	}

	/*
	//should always be bound
	void bind();
	void unbind();
	*/

	void destroy();

};