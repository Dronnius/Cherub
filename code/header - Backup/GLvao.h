#pragma once
#include <glew/glew.h>
#include <glfw/glfw3.h>
//#include <Visual.h>



//Ir MAAAAAY be a misstake to prefix the class name with "GL", since it is used by datatypes, such as GLuint...
class GLvao
{
private:

	//TODO: Some malloc-esque system to manage buffers with frequently added/removed meshes (NO MEMORY LEAKS)
	//GLuint vertexBufferId;	//will probably add an indexBuffer later (would be nice to initialize like this: "glGenBuffers(2, buf);" where buf is a 2-element GLuint-array)
	//unsigned int vertexDataOffset = 0;	//offset used when loading in new meshes (how much memory has been allocated?)
	//unsigned int totalVertexCount = 0;	//a counter for the number of vertices currently in the buffer,
	//GLuint indexBufferId;
	//unsigned int indexDataOffset = 0;	//offset used when loading in new meshes (how much memory has been allocated?)


public:
	GLuint id;
	//static const unsigned int vertexByteSize = 7 * sizeof(float);	//TEMPORARY STATIC VALUE for number of bytes per vertex, TODO: PROPER IMPLEMENTATION LATER!

	//the highest index value of the currently enabled VertexAttribPointers
	//this variable is accessed and managed by Shader objects
	//At any time (except while managing shaders) this variable MUST be correct according to the description above
		//OR, be valued -1, implying that NO VAPs are enabled.
	int enabledAttribsMaxValue = -1;

	//	This idea is cool, but ignore it for now
	//Genereates n GLVAO objects 
	//static genArrays (unsigned int n)

	GLvao(/*unsigned int vertexBufferSize = sizeof(float) * 10 * 3 * 1000, unsigned int indexBufferSize = 3 * 1000, GLenum mode = GL_STATIC_DRAW*/);
	~GLvao();

	void bind();
	void unbind();

	//load a mesh into the buffer (a Visual.cpp object)
	//void load(Visual mesh);


	//GLuint getVB() { return vertexBufferId; }	//getter for vertexBufferId (NO SETTER!)


};