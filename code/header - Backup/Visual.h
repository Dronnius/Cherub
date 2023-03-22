#pragma once

#include <Transform.h>
#include <GLvao.h>
#include <Shader.h>

//OK NEW PLAN:
//	Visual objects keep references to their of VBOs and IBOs, allocated by the VAO (static fuinction? maybe later)
//	Load() function is in visual object

//TODO: TEXTURES

class Visual : public Transform
{
private:
	//here follows the id of several openGL components


	float* vertices = nullptr;				//pointer to a malloc() allocated float array
	unsigned int vertexSize = 0;	//size in bytes, of the vertices array
	//unsigned int vertexCount = 0;	//number of vertex elements represented by the vertices array
	unsigned int* indices = nullptr;			//pointer to a malloc() allocated float array
	unsigned int indexSize = 0;		//size in bytes, of the indices array
	unsigned int indexCount = 0;	//number of values in the index array

	bool heapedVertices = false;	//track if destructor needs to free
	bool heapedIndices = false;	//as above

	GLuint vertexBuffer = 0;	//handle of openGL obejct
	GLuint indexBuffer = 0;		//as above

	//Merely a pointer, shader is created outside, shared between several visual entities. Should probably be allocated on the heap
	Shader* shader = nullptr;	//shader to be used during draw calls (set during load(), default to some debug shader?)
	//glm::mat4* camera = nullptr;	//projection + view matrix, set in draw function prior to shader prep, read by shader prep

public:
	Visual() {};	//these will need to do stuff
	~Visual() 
	{
		if (heapedVertices)
			free(vertices);

		if (heapedIndices)
			free(indices);
	};	// this one too

	void setShader(Shader* shader)
	{
		this->shader = shader;
	}

	/*glm::mat4* getCamera()
	{
		return this->camera;
	}*/

	//returns true -> success, false -> failure
	//overload with different sources of data (pointers, files...)
	//drawModes: GL_STATIC_DRAW or 
	bool load(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount, GLvao* vao, Shader* shaderInput, GLenum drawMode = GL_STATIC_DRAW);

	// Render function
	// camera argument is the view * projection matrix
	// uniformHandle argument SHOULD NOT be here, fetch from shader object instead (somehow)
	bool draw(/*glm::mat4* camera,*/ GLvao* vao);

	static void debug_unitTest(GLFWwindow*);
};