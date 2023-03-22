#include <Shader.h>
#include <Debug.h>
#include <Visual.h>
#include <Cameraman.h>


//VERTEX DATA FORMAT:
//3 floats position
//2 floats texture coordinates
//1 float texture unit
class TextureOnlyShader : public Shader
{

private:
	GLint modelMatrixLocation;
	GLint textureLocation;

protected:

	//return highest index of attributes used
	int prepAttribs(Visual* target) override
	{
		//LOG("prepAttribs called");
		glEnableVertexArrayAttrib(Painter::getVaoID(), 0);
		glEnableVertexArrayAttrib(Painter::getVaoID(), 1);
		//glEnableVertexArrayAttrib(vao->id, 2);

		//stride (2nd last arg) is distance in bytes from start of first vertext to start of second (total size of one vertex' data)
		//offset (last arg, actually an int) distance in bytes from start of vertex data to component data (total size of preceeding components)
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glVertexAttribPointer(2, 1, GL_FLOAT, false, 6 * sizeof(float), (void*)(5 * sizeof(float)));

		if (!CHECK("prepAttribs"))
			return -1;
		else return 1;
	}

	//this function should set uniform values
	//uniform locations are fetched once, in the constructor
	bool prepUniforms(Visual* target) override
	{
		//modelMatrixLocation = glGetUniformLocation(this->id, "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
		const GLfloat* mvpMatrix = glm::value_ptr(*Cameraman::film() * *target->getWorldMatrix());	//get matrix
		glUniformMatrix4fv(this->modelMatrixLocation, 1, GL_FALSE, mvpMatrix);	//register changes (send to shader)

		glUniform1i(textureLocation, 0);		//TEXTURE UNITS ARE FOR NORMAL MAPS AND SUCH FANCY SHIT, NOT FOR PICKING DIFFERENT IMAGES
		
		if (!CHECK("prepUniforms"))
		{
			return false;
		}
		else return true;
	}

public:
	//uniform locations are fetched once, here in the constructor
	TextureOnlyShader() : Shader("assets/shaders/vertex/vs_texture_transform.shader", "assets/shaders/fragment/fs_texture.shader")
	{
		modelMatrixLocation = glGetUniformLocation(this->id, "modelMatrix");	//ASSUMING ID HAS BEEN SET BY SUPER-CLASS CONSTRUCTOR ALREADY, should be the case...
		textureLocation = glGetUniformLocation(this->id, "textureSampler");
	}
};