//#include <glew/glew.h>
//#include <glfw/glfw3.h>
#include <iostream>
#include "Debug.h"
#include "DebugConsoleCommand.h";	//just for Command definition (only include in classes that want to add commands) (also includes <vector>)
#include <thread> //I know, I know, I don't like it either. But I have to in order to get console inputs to work. I'll try to minimise impact on the rest of the program
#include <mutex>	//just for locking latesCommand vector


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


//commandList data
//int commandListContainerLength = 0;
//int commandListActiveLength = 0;
std::mutex listLock;
std::vector<Command> commandList = std::vector<Command>();

//console Watcher data
std::thread watcher;
bool keepWatching = true;	//set to false to kill watcher
int maxCommandArgCharacters = 100;
std::mutex commandLock;	//lock on latestCommand
std::vector<std::string> latestCommand;			//note that despite being a vector, this variable holds only a single command at a time
bool (*latestCommandFunction)(std::vector<std::string>) = nullptr;


bool commandHelp(std::vector<std::string> args)
{
	for (int i = 0; i < commandList.size(); i++)
	{
		std::cout << commandList[i].name << "\t\t" << commandList[i].desc << std::endl;
	}
	
	return true;
}

bool commandEcho(std::vector<std::string> args)
{
	for (int i = 1; i < args.size(); i++)
	{
		std::cout << args[i] << " ";
	}
	std::cout << std::endl;
	return true;
}


//this runs on a separate thread
//try to minimise active execution
//do not call commands from here, this risks race conditions
//pass instructions back to main thread (polled with checkConsoleCommands
//TODO: FIND A WAY TO KILL THE THREAD WHEN IT'S STARTED WAITING FOR INPUT
//TODO: You might want to lower the priority of this thread too
void consoleWatcher()
{
	while (keepWatching)
	{
		//char* input = new char[maxCommandArgCharacters];
		////blocks execution
		//int fields = scanf_s("%s", input, maxCommandArgCharacters);
		////std::cout << input << std::endl;
		
		//read input
		std::string input;
		std::getline(std::cin, input);

		//when new input is detected

		//do some processing
		//char* inputName = splitCstring(input, '\n'); //retrieve function name
		std::vector<std::string> parsedInput = splitString(input, " ");
		//LOG("input: " + input + "\n\tparsed: ");		//DEBUG LOGGING
		//for (std::string s : parsedInput)				//DEBUG LOGGING
		//	LOG("\t\t" + s);							//DEBUG LOGGING
		if (parsedInput.size() < 0)
			continue;	//no arguments, something is wrong, loop

		bool watcherExecution = false;
		if(parsedInput[0].compare("watcher") == 0|| parsedInput[0].compare("Watcher") == 0)		//preceed command with "watcher" to force watcher thread to execute the commmand
			if (parsedInput.size() >= 2)
			{
				watcherExecution = true;
				parsedInput.erase(parsedInput.begin());
			}
			else
			{
				std::cout << "watcher needs a second argument to execute" << std::endl;
			}

		std::string inputName = parsedInput[0];
		bool (*function)(std::vector<std::string>) = nullptr;	//pointer to command function, cast later


		listLock.lock();
		for (int i = 0; i < commandList.size() && function == nullptr; i++)	//loop until a match is found
		{
			if (commandList[i].name.compare(inputName) == 0)
			{
				//found match
				function = commandList[i].func;
			}
		}
		listLock.unlock();
		if (function == nullptr)	//if no match was found, look for new input
		{
			std::cout << "Command not recongised: " << inputName <<std::endl;
			continue;
		}
		//std::vector<std::string> newCommand = std::vector<std::string>();
		//newCommand.push_back(std::string(inputName));
		//while(input != nullptr)
		//{
		//	newCommand.push_back(std::string(splitCstring(input, '\n')));	//possible issue here, if splitCstring returns nullptr, just do a check beforehand if this occurs
		//}
		
		if (watcherExecution)			//if watcher execution forced
		{
			commandLock.lock();
			LOGNOTE("Watcher thread executing command...");
			//act on command
			if (!function(parsedInput))
			{
				LOGWARNING("Console command executed by watcher returned failure");
			}
			commandLock.unlock();
		}
		else
		{
			//We have a new input, overwrite the old!
			//LOGNOTE("New console command parsed, grabbing command lock");
			commandLock.lock();
			latestCommand.clear();	//just in case
			latestCommand = parsedInput;
			latestCommandFunction = function;
			commandLock.unlock();
			//LOGNOTE("Command lock released");
		}
	}
}

void initialiseConsoleCommands()
{
	LOGNOTE("Initialising Console commands");
	//add commands without wrapper function to avoid circular call from wrapper function
	listLock.lock();
	commandList.push_back(Command("help", commandHelp, "Display list of all available console-commands."));
	commandList.push_back(Command("echo", commandEcho, "Print out given arguments."));
	listLock.unlock();
	watcher = std::thread(consoleWatcher);	//launcher watcher thread
}

//call this function at the start of each frame
void checkConsoleCommands(const float deltaTime)
{
	if (commandList.empty())	//IF Initialization needed
		initialiseConsoleCommands();

	//Check for input
	if (listLock.try_lock())
	{
		if (!latestCommand.empty())
		{
			listLock.unlock();
			if (commandLock.try_lock())	//continue normal execution if command editing is in process
			{
				//act on command
				if (!latestCommandFunction(latestCommand))
				{
					LOGWARNING("Console command returned failure");
				}
				latestCommand.clear();	//remove command
				latestCommandFunction = nullptr;
				commandLock.unlock();
			}
		}
		else
			listLock.unlock();
	}
}

//call this before quitting program, and ONLY then
void consoleWatcherDestroy()
{
	keepWatching = false;
	LOGWARNING("Waiting for watcher to terminate \n if the program hangs here, try hitting enter in the console\nI really should fix this...");
	watcher.join();
	LOGNOTE("Watcher terminated successfully");
}


//declared in DebugConsoleCommand.h
void addToConsoleCommandList(Command c)
{
	if (commandList.empty())	//IF Initialization needed
		initialiseConsoleCommands();
	listLock.lock();
	commandList.push_back(c);
	listLock.unlock();
}

//just a little helper-function that splits c-strings by groups of the delimiter character
//modifies the input by nulling a character moving the pointer further along
//returns the start of the first split, essentially, it returns the old string pointer (unless there are delimiters at the start
//returns nullptr on various invalid results, also sets string to nullptr
char* splitCstring(char* string, char delimiter)
{
	char* subtextStart = nullptr;
	if (delimiter == '\0' || string == nullptr)	//just in case
	{
		subtextStart = string;
		string = nullptr;
		return subtextStart;
	}

	int c = 0;
	//skip inital delimiters
	for (; string[c] == delimiter; c++)
		continue;

	if (string[c] == '\0')	//if next character is null
	{
		string = nullptr;
		return nullptr;
	}
	subtextStart = string + c;	//here is the start of the subtext

	//look for next delimiter
	for (; string[c] != delimiter && string[c] != '\0'; c++)
		continue;

	for (; string[c] == delimiter; c++)//replace all delimiters with termination characters
	{
		string[c] = '\0';
	}
	//by this point, the string[c] is either text we want to keep, or a termination character
	if (string[c] == '\0')	//end of text
	{
		string = nullptr;
		return subtextStart;
	}
	else
	{
		string = string + c;
		return subtextStart;
	}
}

//Credit to herohuyongtao of stackoverflow: https://stackoverflow.com/questions/20755140/split-string-by-a-character
std::vector<std::string> splitString(const std::string text, const std::string delimeter)		//bool unifyQuotes (later)
{
	int textLen = text.length();
	std::vector<std::string> substrings;
	int position = 0;
	while (position < textLen)
	{
		std::string sub = text.substr(position, text.find(delimeter, position) - position);		//consider find_first_of instead https://cplusplus.com/reference/string/string/find_first_of/
		position += sub.length() + delimeter.length();
		substrings.push_back(sub);	
	}
	return substrings;
}


//we'll asume that strings are already trimmed
//all these functions throw invalid argument on failure
//int sParseInt(std::string input)
//{
//	return std::stoi(input);
//}
//float sParseFloat(std::string input)
//{
//	return std::stof(input);
//}
glm::vec3 sParseVec3(std::string input)
{
	std::vector<std::string> components = splitString(input, ",");
	glm::vec3 ret(0.0f);
	for (int i = 0; i < 3 && i < components.size(); i++)
	{
		try { ret[i] = std::stof(components[i]); }
		catch (std::invalid_argument& e) { LOGWARNING("failed to parse component " + std::to_string(i) + " of " + input); throw e; }
	}
	return ret;
}
glm::vec4 sParseVec4(std::string input)
{
	std::vector<std::string> components = splitString(input, ",");
	glm::vec3 ret(0.0f);
	for (int i = 0; i < 4 && i < components.size(); i++)
	{
		try { ret[i] = std::stof(components[i]); }
		catch (std::invalid_argument& e) { LOGWARNING("failed to parse component " + std::to_string(i) + " of " + input); throw e; }
	}
}