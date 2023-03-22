//#include <glew/glew.h>
//#include <glfw/glfw3.h>
#include <iostream>
#include "Debug.h"



/////////////////DEBUG


//Setting console colours: http://www.infernodevelopment.com/set-console-text-color
#ifdef _WIN32	//https://iq.opengenus.org/detect-operating-system-in-c/
#include <Windows.h>
HANDLE conHandle;
void setColour(logType colour)	//LOG_NOTE to reset to white
{
	if (conHandle == NULL)
	{
		conHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (conHandle == NULL)
		{
			std::cout << "EROOR from " << __FILE__ << ":" << __LINE__ << "setColour() failed, handle is NULL" << std::endl;
			return;
		}
	}
	switch (colour)
	{
	case logType::LOG_SIMPLE:
		SetConsoleTextAttribute(conHandle, FOREGROUND_GREEN);
		break;
	case logType::LOG_NOTE:
		SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		break;
	case logType::LOG_WARNING:
		SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case logType::LOG_ALERT:
		SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case logType::LOG_ERROR:
		SetConsoleTextAttribute(conHandle, FOREGROUND_RED);
		break;
	case logType::LOG_FATAL:
		SetConsoleTextAttribute(conHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case logType::LOG_OPENGL:
		SetConsoleTextAttribute(conHandle, FOREGROUND_BLUE);
		break;
	}
}
#endif
#ifndef _WIN32
void setColour(logType colour) {}	//dud
#endif

void clearError()    //discard all remaining errors
{
	while (glGetError() != GL_NO_ERROR);
}

bool checkError(const char* function, const char* file, int line)
{
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR)
	{
		setColour(logType::LOG_OPENGL);
		std::cout << "[OpenGL Error] (" << error << "): " << function << "() in " << file << ":" << line << std::endl;
		setColour(logType::LOG_NOTE);
		return false;
	}
	return true;
}

void GLAPIENTRY debugResponse(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	setColour(logType::LOG_OPENGL);
	std::cout << "OpenGL Error-response: [" << id << "] - \"" << message << "\"" << std::endl;
	setColour(logType::LOG_NOTE);
}


//TODO: Also print to textfile perhaps?
void log(logType type, const std::string message, const char* file, const int line)
{
	setColour(type);
	switch (type)
	{
	case logType::LOG_SIMPLE:
		std::cout << message << std::endl;
		break;
	case logType::LOG_NOTE:
		std::cout << "NOTE from " << file << ":" << line << "\n\t" << message << std::endl;
		break;
	case logType::LOG_WARNING:
		std::cout << "WARNING from " << file << ":" << line << "\n\t" << message << std::endl;
		break;
	case logType::LOG_ALERT:
		std::cout << "ALERT from " << file << ":" << line << "\n\t" << message << std::endl;
		break;
	case logType::LOG_ERROR:
		std::cout << "ERROR from " << file << ":" << line << "\n\t" << message << std::endl;
		break;
	case logType::LOG_FATAL:
		std::cout << "FATAL from " << file << ":" << line << "\n\t" << message << std::endl;
		break;
	}
	setColour(logType::LOG_NOTE);
}
//////////////////////