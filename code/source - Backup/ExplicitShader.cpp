#include <Shader.h>
#include <iostream>
#include <Debug.h>
#include <GLvao.h>
#include <Visual.h>
#include <Cameraman.h>

class ExplicitShader : public Shader
{

private:
	GLint modelMatrixLocation;

protected:
	
	//return highest index of attributes used
	int prepAttribs(Visual* target, GLvao* vao) override
	{
		//std::cout << "prepAttribs called" << std::endl;
		glEnableVertexArrayAttrib(vao->id, 0);
		glEnableVertexArrayAttrib(vao->id, 1);

		//stride (2nd last arg) is distance in bytes from start of first vertext to start of second (total size of one vertex' data)
		//offset (last arg, actually an int) distance in bytes from start of vertex data to component data (total size of preceeding components)
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 7 * sizeof(float), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		
		if (!CHECK("prepAttribs"))
			return -1;
		else return 1;
	}

	bool prepUniforms(Visual* target) override
	{
		//modelMatrixLocation = glGetUniformLocation(this->id, "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
		//const GLfloat* mvpMatrix = glm::value_ptr(*camera * *this->getMatrix());
		const GLfloat* mvpMatrix = glm::value_ptr(*Cameraman::film() * *target->getMatrix());	//get matrix
		glUniformMatrix4fv(this->modelMatrixLocation, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)
		if (!CHECK("prepUniforms"))
			return false;
		else return true;
	}
	
public:
	ExplicitShader() : Shader("assets/shaders/vertex/vs_transform_passColour.shader", "assets/shaders/fragment/fs_explicitColour.shader")
	{
		modelMatrixLocation = glGetUniformLocation(this->getID(), "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
	}
};