#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <fstream>
#include <iostream>

#include "Shader.h"
#include "Visual.h"
#include "Debug.h"

Shader::Shader(const std::string vertexShader, const std::string fragmentShader)
{
    //create object
    id = glCreateProgram();
    std::cout << "created shader id = " << id << std::endl;

    //call compile shader twice
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //attatch compiled shaders to program
    glAttachShader(id, vs);
    glAttachShader(id, fs);

    //Hey, listen! You have to LINK first!
    glLinkProgram(id);

    //check link-status
    //Copied from The Cherno (Well, most of this is more or less...)
    // Error handling here, retrieve
    int result = GL_FALSE;
    // Documentation: http://docs.gl/gl4/glGetShader
    glGetProgramiv(id, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) //linking failed
    {
        int length, returnlen;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char));
        //Documentation: http://docs.gl/gl4/glGetShaderInfoLog
        glGetProgramInfoLog(id, length, &returnlen, message);
        std::cout << "Failed to link: " << std::endl << message << std::endl;
        free(message);
    }
    else std::cout << "Linked successfully!" << std::endl << "\t" << vertexShader << " and " << fragmentShader << std::endl;

    //finally, validate (A DEVELOPMENT-ONLY THING)
    glValidateProgram(id);
    result = GL_FALSE;
    glGetProgramiv(id, GL_VALIDATE_STATUS, &result);
    if (result == GL_FALSE) //linking failed
    {
        int length, returnlen;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char));
        //Documentation: http://docs.gl/gl4/glGetShaderInfoLog
        glGetProgramInfoLog(id, length, &returnlen, message);
        std::cout << "Failed to validate: " << std::endl << message << std::endl;
        free(message);
    }
    else std::cout << "Validated successfully!" << std::endl;

    //detatch for some reason
    glDetachShader(id, vs);
    glDetachShader(id, fs);

    //delete source code
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::bind()
{
    //std::cout << "about to use program #" << id << std::endl;
    glUseProgram(id);
    //CHECK("bind");
}

void Shader::unbind()
{
    glUseProgram(0);
}

//responsible delegating for attribute layout
//performs essential tasks, then passes on responsibility to prepAttribs() and prepUniforms()
//true = success, false = failure, depends on the prepUniforms function
bool Shader::prep(Visual* target, GLvao* vao)
{
    //bind vao, just to be safe
    vao->bind();


    //disable all attributes, just to be extra safe, only works if vao.enabledAttribsMaxValue is correctly set
    for (int i = vao->enabledAttribsMaxValue; i >= 0; i--)
    {
        glDisableVertexAttribArray(i);
        if (!CHECK("prep"))
        {
            std::cout << "enabledAttribsMaxValue probably incorrectly set, current value = " << vao->enabledAttribsMaxValue << std::endl;
        }
    }

    //prepare new attributes, and update variable accordingly
    vao->enabledAttribsMaxValue = this->prepAttribs(target, vao);

    //bind this shader, because that is important
    this->bind();

    //prepare uniforms
    return this->prepUniforms(target);
}

GLuint Shader::compileShader(unsigned int type, std::string shaderSource)
{

    //DEBUG:
    if (type == GL_VERTEX_SHADER)
        std::cout << std::endl << "Compiling Vertex Shader: " << shaderSource << "..." << std::endl;
    else if (type == GL_FRAGMENT_SHADER)
        std::cout << std::endl << "Compiling Fragment Shader: " << shaderSource << "..." << std::endl;
    else
        std::cout << std::endl << "Compiling Unknown Shader: " << shaderSource << "..." << std::endl;

	//Copied from Maik Beckmann @ Stack Overflow
	std::ifstream stream(shaderSource);
	std::string shader;
	shader.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()); //strange arguments eh?

	GLuint shaderID = glCreateShader(type);
	const char* shaderString = shader.c_str();
	glShaderSource(shaderID, 1, &shaderString, nullptr);	//last element should be length of string. nullptr will only work if string is NULL-terminated. Is it?
	glCompileShader(shaderID);

	//Copied from The Cherno (Well, most of this is more or less...)
    // Error handling here, retrieve
    int result = GL_FALSE;
    // Documentation: http://docs.gl/gl4/glGetShader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) //compilation failed
    {
        int length, returnlen;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)malloc(length * sizeof(char));
        //Documentation: http://docs.gl/gl4/glGetShaderInfoLog
        glGetShaderInfoLog(shaderID, length, &returnlen, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl << message << std::endl;
        free(message);
    }

    //DEBUG:
    if (type == GL_VERTEX_SHADER)
        std::cout << "Compiled Vertex Shader: " << shaderSource << std::endl;
    else if(type == GL_FRAGMENT_SHADER)
        std::cout << "Compiled Fragment Shader: " << shaderSource <<  std::endl;
    else
        std::cout << "Compiled Unknown Shader: " << shaderSource << std::endl;
        
    std::cout << shaderString << std::endl;

    return shaderID;
}