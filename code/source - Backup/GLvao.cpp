#include <GLvao.h>
#include <debug.h>

#include <iostream>

GLvao::GLvao(/*unsigned int vertexBufferSize, unsigned int indexBufferSize, GLenum mode*/)
{
	//Retrieve an ID, and create it too (I think)
	glGenVertexArrays(1, &id);
	CHECK("Constructor");	//DEBUG

}

GLvao::~GLvao()
{
	glDeleteVertexArrays(1, &id);
	CHECK("Destructor");	//DEBUG
}

void GLvao::bind()
{
	glBindVertexArray(id);
	CHECK("bind");		//DEBUG
}

void GLvao::unbind()
{
	glBindVertexArray(0);
	CHECK("unbind");	//DEBUG
}

/*
//load a mesh into the buffer (a Visual.cpp object)
//TODO: Handle index data also
//TODO: Visual object must contain an object describing the layout of the data
//TODO: Some kind of error detection, in case new mesh doesn't fit in the buffer
void GLvao::load(Visual mesh)
{

	//get values from mesh
	float* vertices = nullptr;
	unsigned int* indices = nullptr;
	unsigned int vertexSize, vertexCount, indexSize, indexCount;
	mesh.getMesh(&vertices, &vertexSize, &vertexCount, &indices, &indexSize, &indexCount);

	GLuint vertexBuffer, indexBuffer;
	//Bind this vertex array object

	CHECK("load");		//DEBUG
}
*/