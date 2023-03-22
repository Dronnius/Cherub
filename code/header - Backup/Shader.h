#pragma once
#include <string>
#include <glew/glew.h>
//#include <glfw/glfw3.h>

class Visual;	//forward declaration to avoid circular dependency
class GLvao;

//abstract, subclasses MUST set up the shader based on subclass-specific shaders
	//they MUST also override functions prepAttrics and prepUniforms.
class Shader
{
private:
	GLuint id;

	GLuint compileShader(unsigned int type, std::string shader);

protected:

	//MUST BE OVERRIDDEN
	//handles enabling and creation of vertexAttribPointers, called for each switch of shaders
	//must return the highest index value of currently enabled VAPs, or -1 if none are enabled
	virtual int prepAttribs(Visual* target, GLvao* vao) = 0;

	//MUST BE OVERRIDDEN
	//handles enabling and creation of vertexAttribPointers, called for each switch of shaders
	virtual bool prepUniforms(Visual* target) = 0;


public:
	Shader(const std::string vertexShader, const std::string fragmentShader);
	~Shader();

	void bind();
	void unbind();

	GLuint getID()
	{
		return id;
	}

	//responsible delegating for attribute layout
	//performs essential tasks, then passes on responsibility to prepAttribs() and prepUniforms()
	bool prep(Visual* target, GLvao* vao);
};