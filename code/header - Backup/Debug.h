#pragma once
#include <glew/glew.h>
#include <glfw/glfw3.h>
//#include <iostream>
#include <string>

#define CHECK(function) checkError(function, __FILE__, __LINE__)	//CHECK returns true if everything is OK, a typical call would be if(!CHECK("function"))
#define VERIFY(function) if(!checkError(function, __FILE__, __LINE__)) exit(EXIT_FAILURE)

void clearError();    //discard all remaining errors

bool checkError(const char* function, const char* file, int line);

void GLAPIENTRY debugResponse(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


//LOGGING:
//Use these macros instead of actual function calls
enum logType { LOG_SIMPLE = -1, LOG_NOTE = 0, LOG_WARNING = 1, LOG_ALERT = 2, LOG_ERROR = 3, LOG_FATAL = 4 , LOG_OPENGL = 5};
#define LOG(message)		log(LOG_SIMPLE, message, "\0", -1)	//for pure text, mostly for unit testing purposes
#define LOGNOTE(message)	log(LOG_NOTE, message, __FILE__, __LINE__)			//For simple information, nothing went wrong, just providing information...
#define LOGWARNING(message) log(LOG_WARNING, message, __FILE__, __LINE__)		//For unwanted behaviour, not an immenent issue however
#define LOGALERT(message)	log(LOG_ALERT, message, __FILE__, __LINE__)		//For unwanted behaviour that is likely to cause issues down the line, but not right this moment
#define LOGERROR(message)	log(LOG_ERROR, message, __FILE__, __LINE__)		//Something went wrong, a feature failed to perform
#define LOGFATAL(message)	log(LOG_FATAL, message, __FILE__, __LINE__)		//Something vital went wrong, the program will crash

//using std::string instead of char* allows string concatenation in argument, it seems
void log(logType type, const std::string message, const char* file, const int line);