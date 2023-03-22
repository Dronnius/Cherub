#include <Shader.h>
#include <Debug.h>
#include <Visual.h>
#include <Cameraman.h>


//VERTEX DATA FORMAT:
//3 floats position
//4 floats colour (RGBA)
class ExplicitShader : public Shader
{

private:
	GLint modelMatrixLocation;

protected:
	
	//return highest index of attributes used
	int prepAttribs(Visual* target) override
	{
		//std::cout << "prepAttribs called" << std::endl;
		glEnableVertexArrayAttrib(Painter::getVaoID(), 0);
		glEnableVertexArrayAttrib(Painter::getVaoID(), 1);

		//stride (2nd last arg) is distance in bytes from start of first vertext to start of second (total size of one vertex' data)
		//offset (last arg, actually an int) distance in bytes from start of vertex data to component data (total size of preceeding components)
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		
		if (!CHECK("prepAttribs"))
			return -1;
		else return 1;
	}

	bool prepUniforms(Visual* target) override
	{
		//modelMatrixLocation = glGetUniformLocation(this->id, "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
		//const GLfloat* mvpMatrix = glm::value_ptr(*camera * *this->getMatrix());
		const GLfloat* mvpMatrix = glm::value_ptr(*Cameraman::film() * *target->getWorldMatrix());	//get matrix
		glUniformMatrix4fv(this->modelMatrixLocation, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)
		if (!CHECK("prepUniforms"))
			return false;
		else return true;
	}
	
public:
	ExplicitShader() : Shader("assets/shaders/vertex/vs_transform_passColour.shader", "assets/shaders/fragment/fs_explicitColour.shader")
	{
		modelMatrixLocation = glGetUniformLocation(this->id, "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
	}
};